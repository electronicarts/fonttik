#include "TinEye.h"
#include <iostream>
#include <string>
#include "Configuration.h"
#include "Image.h"
#include "TextboxDetection.h"
#include "boost/log/trivial.hpp"
#include "Guideline.h"
#include "AppSettings.h"
#include "TextDetectionParams.h"
#include "Instrumentor.h"

namespace tin {
	std::vector<double>* TinEye::linearizationLUT = nullptr;

	Results* TinEye::processMedia(Media& media) {
		do {
			//Check if image has text recognized by OCR
			applyFocusMask(media);
			std::vector<tin::Textbox> textBoxes = getTextBoxes(media);
			mergeTextBoxes(textBoxes);
			if (textBoxes.empty()) {
				BOOST_LOG_TRIVIAL(info) << "No words recognized in image" << std::endl;
			}
			else {
				// Get OCR result
				fontSizeCheck(media, textBoxes);
				textContrastCheck(media, textBoxes);
			}
		} while (media.nextFrame());

		return media.getResultsPointer();
	}

	void TinEye::init(Configuration* configuration)
	{
		Instrumentor::Get().BeginSession("Profile");
		PROFILE_FUNCTION();
		config = configuration;

		//load LUT for rgb linearization
		linearizationLUT = config->getRGBLookupTable();

		//Initialize EAST detection
		textboxDetection = new TextboxDetection(config->getTextDetectionParams());


		//Initialize text recognition only if text recognition is enabled in config

		//from https://docs.opencv.org/4.x/d4/d43/tutorial_dnn_text_spotting.html
		if (config->getAppSettings()->textRecognitionActive()) {
			// Load models weights
			TextRecognitionParams* recognitionParams = config->getTextRecognitionParams();
			textRecognition = cv::dnn::TextRecognitionModel(recognitionParams->getRecognitionModel());
			textRecognition.setDecodeType(recognitionParams->getDecodeType());
			std::ifstream vocFile;
			vocFile.open(recognitionParams->getVocabularyFilepath());
			CV_Assert(vocFile.is_open());
			std::string vocLine;
			std::vector<std::string> vocabulary;
			while (std::getline(vocFile, vocLine)) {
				vocabulary.push_back(vocLine);
			}
			textRecognition.setVocabulary(vocabulary);

			// Normalization parameters
			auto mean = recognitionParams->getMean();
			// The input shape
			std::pair<int, int> size = recognitionParams->getSize();
			textRecognition.setInputParams(recognitionParams->getScale(), cv::Size(size.first, size.second), cv::Scalar(mean[0], mean[1], mean[2]));
		}
	}

	void TinEye::applyFocusMask(Media& image) {
		PROFILE_FUNCTION();
		cv::Mat img = image.getImageMatrix();
		cv::Mat mask = config->getAppSettings()->calculateMask(img.cols, img.rows);
		img = img & mask;
	}

	bool TinEye::fontSizeCheck(Media& img, std::vector<Textbox>& boxes) {
		PROFILE_FUNCTION();
		cv::Mat openCVMat = img.getImageMatrix();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();

		if (openCVMat.empty())
		{
			return false;
		}

		guideline->setActiveResolution(openCVMat.rows);

		bool passes = true;

#ifdef _DEBUG
		int counter = 0;
#endif

		//add entry for this image in result struct
		Results* testResults = img.getResultsPointer();
		testResults->sizeResults.push_back(std::vector<ResultBox>());

		for (Textbox box : boxes) {
			//Set word detection to word bounding box
			box.setParentMedia(&img);

			bool individualPass = textboxSizeCheck(img, box);

			passes = passes && individualPass;

#ifdef _DEBUG
			if (appSettings->saveSeparateTextboxes()) {
				img.saveOutputData(box.getSubmatrix(), "textbox_" + std::to_string(counter) + ".png");
			}
			counter++;

#endif
		}

		return passes;
	}

	bool TinEye::textboxSizeCheck(Media& image, const Textbox& textbox) {
		PROFILE_FUNCTION();
		bool pass = true;
		cv::Rect boxRect = textbox.getRect();

		ResultType type = ResultType::PASS;

		if (config->getAppSettings()->textRecognitionActive()) {
			//Recognize word in region
			std::string recognitionResult;
			recognitionResult = textRecognition.recognize(textbox.getSubmatrix());

			//Avoids division by zero
			int averageWidth = (recognitionResult.size() > 0) ? boxRect.width / recognitionResult.size() : -1;

			//Check average width
			if (averageWidth == -1) {
				BOOST_LOG_TRIVIAL(warning) << "Text inside " << boxRect << " couldn't be recognized, it is suggested to increase the text recognition minimum confidence" << std::endl;
				type = ResultType::UNRECOGNIZED;
			}
			else if (averageWidth < config->getGuideline()->getWidthRequirement()) {
				pass = false;
				BOOST_LOG_TRIVIAL(info) << "Average character width for word: " << recognitionResult << " doesn't comply with minimum width, detected width: " << averageWidth <<
					" at (" << boxRect.x << ", " << boxRect.y << ")" << std::endl;
			}
		}


		//Check height
		int minimumHeight = config->getGuideline()->getHeightRequirement();
		if (boxRect.height < minimumHeight) {
			pass = false;
			BOOST_LOG_TRIVIAL(info) << "Word at (" << boxRect.x << ", " << boxRect.y << ") doesn't comply with minimum height " 
				<< minimumHeight << ", detected height : " << boxRect.height << std::endl;
		}

		type = (pass) ? type : ResultType::FAIL;
		Results* testResults = image.getResultsPointer();
		testResults->sizeResults.back().push_back(ResultBox(type, boxRect.x, boxRect.y, boxRect.width, boxRect.height));
		testResults->overallSizePass = testResults->overallSizePass && pass;

		return pass;
	}

	bool TinEye::textContrastCheck(Media& image, std::vector<Textbox>& boxes) {
		PROFILE_FUNCTION();

		cv::Mat openCVMat = image.getImageMatrix();
		cv::Mat luminanceMap = image.getFrameLuminance();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();

#ifdef _DEBUG
		int counter = 0;
#endif // _DEBUG


		if (openCVMat.empty())
		{
			return false;
		}

		bool imagePasses = true;

		//add entry for this image in result struct
		Results* testResults = image.getResultsPointer();
		testResults->contrastResults.push_back(std::vector<ResultBox>());

		for (Textbox box : boxes) {

			box.setParentMedia(&image);

			bool individualPass = textboxContrastCheck(image, box);


#ifdef _DEBUG
			if (appSettings->saveHistograms()) {
				PROFILE_SCOPE("saveHistograms");
				cv::Rect boxRect = box.getRect();
				fs::path savePath = image.getPath().replace_filename("img" + std::to_string(counter) + "histogram.png").string();
				Image::saveLuminanceHistogram(box.getLuminanceHistogram(),
					savePath.string());

				Image::saveHistogramCSV(image.calculateLuminanceHistogram(boxRect), image.getPath().replace_filename("histogram" + std::to_string(counter) + ".csv").string());
			}
			counter++;

#endif
			imagePasses = imagePasses && individualPass;

		}

		return imagePasses;
	}

	bool TinEye::textboxContrastCheck(Media& image, const Textbox& box) {
		PROFILE_FUNCTION();
		cv::Rect boxRect = box.getRect();

		//Contrast checking with thresholds
		cv::Mat luminanceRegion = box.getLuminanceMap();
		cv::Mat maskA, maskB;

		//OTSU threshold automatically calculates best fitting threshold values
		cv::Mat unsignedLuminance;
		luminanceRegion.convertTo(unsignedLuminance, CV_8UC1, 255);
		cv::threshold(unsignedLuminance, maskA, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);
		//Dilate and then substract maskA to get the outline of the mask
		cv::dilate(maskA, maskB, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,7)));
		maskB -= maskA;

#ifdef _DEBUG
		if (config->getAppSettings()->saveLuminanceMasks()) {
			image.saveOutputData(unsignedLuminance, "lum.png");
			image.saveOutputData(maskA, "mask.png");
			image.saveOutputData(maskB, "outlineMask.png");
		}
#endif // _DEBUG

		double ratio = ContrastBetweenRegions(luminanceRegion, maskA, maskB);

		bool boxPasses = ratio >= config->getGuideline()->getContrastRequirement();

		if (!boxPasses) {
			BOOST_LOG_TRIVIAL(info) << "Word: " << boxRect << " doesn't comply with minimum luminance contrast " << config->getGuideline()->getContrastRequirement()
				<< ", detected contrast ratio is " << ratio << " at: " << boxRect << std::endl;
		}

		ResultType type = (boxPasses) ? ResultType::PASS : ResultType::FAIL;
		Results* testResults = image.getResultsPointer();
		testResults->contrastResults.back().push_back(ResultBox(type, boxRect.x, boxRect.y, boxRect.width, boxRect.height));
		testResults->overallContrastPass = testResults->overallContrastPass && boxPasses;

		return boxPasses;
	}

	double TinEye::ContrastBetweenRegions(const cv::Mat& luminanceMap, const cv::Mat& maskA, const cv::Mat& maskB) {
		//Calculate the mean of the luminance for the light regions of the luminance
		double meanLight = Media::LuminanceMeanWithMask(luminanceMap, maskA);

		//Invert mask to calculate mean of the darker colors
		double meanDark = Media::LuminanceMeanWithMask(luminanceMap, maskB);

		return (std::max(meanLight, meanDark) + 0.05) / (std::min(meanLight, meanDark) + 0.05);
	}

	std::vector<Textbox> TinEye::getTextBoxes(Media& image) {
		PROFILE_FUNCTION();
		return textboxDetection->detectBoxes(image.getImageMatrix(), config->getAppSettings(), config->getTextDetectionParams());
	}

	void TinEye::mergeTextBoxes(std::vector<Textbox>& textBoxes) {
		PROFILE_FUNCTION();
		std::pair<float, float> threshold = config->getTextDetectionParams()->getMergeThreshold();
		if (threshold.first < 1.0 || threshold.second < 1.0) {
			TextboxDetection::mergeTextBoxes(textBoxes, config->getTextDetectionParams());
		}
	}

	double TinEye::linearize8bitRGB(const uchar& colorBits) {
		//Profiling this function kills performance
		//PROFILE_FUNCTION();
		//ref https://developer.mozilla.org/en-US/docs/Web/Accessibility/Understanding_Colors_and_Luminance
		double color = colorBits / 255.0;

		if (color <= 0.04045) {
			return color / 12.92;
		}
		else {
			double base = ((color + 0.055) / 1.1055);
			return pow(base, 2.4);
		}
	}

	cv::Mat TinEye::calculateLuminance(cv::Mat imageMatrix) {
		cv::Mat luminanceMap;
		//Make sure that image has been loaded and we haven't previously calculated the luminance already
		if (!imageMatrix.empty()) {
			//Matrix to store linearized rgb
			cv::Mat linearBGR = cv::Mat::zeros(imageMatrix.size(), CV_64FC3);

			//Use look up tables if they have been correctly loaded during initialization
			if (linearizationLUT != nullptr && !linearizationLUT->empty()) {
				cv::LUT(imageMatrix, *linearizationLUT, linearBGR);
			}
			else {
				for (int y = 0; y < imageMatrix.rows; y++) {
					for (int x = 0; x < imageMatrix.cols; x++) {
						cv::Vec3b colorVals = imageMatrix.at<cv::Vec3b>(y, x);

						//TODO lookup table, inexpensive, only 256 values, one for each lum value
						//Could also use three separate lookup tables and merge them into one result directly
						linearBGR.at<cv::Vec3d>(y, x) = {
							linearize8bitRGB(colorVals.val[0]),
							linearize8bitRGB(colorVals.val[1]),
							linearize8bitRGB(colorVals.val[2]) };
					}
				}
			}

			luminanceMap = cv::Mat::zeros(imageMatrix.size(), CV_64FC1); //1 channel (luminance)

			// Possible improvement by having a different LUT for each color channel and then adding themp up with:
			//from https://stackoverflow.com/questions/30666224/how-can-i-turn-a-three-channel-mat-into-a-summed-up-one-channel-mat
			//cv::transform(linearBGR, luminanceMap, cv::Matx13f(1, 1, 1));

			//Adds up all three channels into one luminance value channel 
			for (int y = 0; y < imageMatrix.rows; y++) {
				for (int x = 0; x < imageMatrix.cols; x++) {
					cv::Vec3d lumVals = linearBGR.at<cv::Vec3d>(y, x);
					//BGR order
					luminanceMap.at<double>(y, x) = cv::saturate_cast<double>((lumVals.val[0] * 0.0722 + lumVals.val[1] * 0.7152 + lumVals.val[2] * 0.2126));
				}
			}
		}

		return luminanceMap;
	}

	TinEye::~TinEye()
	{
		if (textboxDetection != nullptr) {
			delete textboxDetection;
		}
		textboxDetection = nullptr;


		config = nullptr;
		Instrumentor::Get().EndSession();
	}
}
