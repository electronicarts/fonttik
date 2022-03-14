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

	void TinEye::init(fs::path configFile)
	{
		Instrumentor::Get().BeginSession("Profile");
		PROFILE_FUNCTION();
		if (config != nullptr) {
			delete config;
		}

		config = new Configuration(configFile.c_str());

		//Initialize EAST detection
		TextboxDetection::init(config->getTextDetectionParams());


		//Initialize text recognition
		//from https://docs.opencv.org/4.x/d4/d43/tutorial_dnn_text_spotting.html

		// Load models weights
		model = cv::dnn::TextRecognitionModel("crnn_cs.onnx");
		model.setDecodeType("CTC-greedy");
		std::ifstream vocFile;
		vocFile.open("alphabet_94.txt");
		CV_Assert(vocFile.is_open());
		std::string vocLine;
		std::vector<std::string> vocabulary;
		while (std::getline(vocFile, vocLine)) {
			vocabulary.push_back(vocLine);
		}
		model.setVocabulary(vocabulary);

		// Normalization parameters
		double scale = 1.0 / 127.5;
		cv::Scalar mean = cv::Scalar(127.5, 127.5, 127.5);
		// The input shape
		cv::Size inputSize = cv::Size(100, 32);
		model.setInputParams(scale, inputSize, mean);
	}

	void TinEye::applyFocusMask(Image& image) {
		PROFILE_FUNCTION();
		cv::Mat img = image.getImageMatrix();
		cv::Mat mask = config->getAppSettings()->calculateMask(img.cols, img.rows);
		img = img & mask;
	}

	bool TinEye::fontSizeCheck(Image& img, std::vector<Textbox>& boxes) {
		PROFILE_FUNCTION();
		cv::Mat openCVMat = img.getImageMatrix();
		cv::Mat luminanceMap = img.getLuminanceMap();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();

		if (openCVMat.empty())
		{
			return false;
		}

		guideline->setActiveResolution(openCVMat.rows);

		bool passes = true;

#ifdef _DEBUG
		//Regions of interest
		cv::Mat ROIs = img.getImageMatrix().clone();
		int counter = 0;
#endif


		for (Textbox box : boxes) {
			//Set word detection to word bounding box
			box.setParentImage(&img);

			bool individualPass = textboxSizeCheck(box);

			passes = passes && individualPass;

#ifdef _DEBUG
			if (appSettings->saveTexboxOutline()) {
				PROFILE_SCOPE("highlight");
				cv::Rect boxRect = box.getRect();
				Image::highlightBox(boxRect.x, boxRect.y, boxRect.x + boxRect.width, boxRect.y + boxRect.height, (individualPass) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), ROIs, 2);
			}
			if (appSettings->saveSeparateTextboxes()) {
				img.saveOutputData(box.getSubmatrix(), "textbox_" + std::to_string(counter) + ".png");
			}
			counter++;

#endif


		}

#ifdef _DEBUG
		if (appSettings->saveTexboxOutline()) {
			img.saveOutputData(ROIs, "sizeChecks.png");
		}
#endif

		return passes;

	}

	bool TinEye::textboxSizeCheck(const Textbox& textbox) {
		PROFILE_FUNCTION();
		bool pass = true;
		cv::Rect boxRect = textbox.getRect();

		//Recognize word in region
		std::string recognitionResult;
		recognitionResult = model.recognize(textbox.getSubmatrix());

		//Avoids division by zero
		int averageWidth = (recognitionResult.size() > 0) ? boxRect.width / recognitionResult.size() : -1;

		//Check average width
		if (averageWidth == -1) {
			BOOST_LOG_TRIVIAL(warning) << "Text inside " << boxRect << " couldn't be recognized, it is suggested to increase the text recognition minimum confidence" << std::endl;
		}
		else if (averageWidth < config->getGuideline()->getWidthRequirement()) {
			pass = false;
			BOOST_LOG_TRIVIAL(info) << "Average character width for word: " << recognitionResult << " doesn't comply with minimum width, detected width: " << averageWidth <<
				" at (" << boxRect.x << ", " << boxRect.y << ")" << std::endl;
		}


		//Check height
		int minimumHeight = config->getGuideline()->getHeightRequirement();
		if (boxRect.height < minimumHeight) {
			pass = false;
			BOOST_LOG_TRIVIAL(info) << "Word: '" << recognitionResult << "' doesn't comply with minimum height " << minimumHeight << ", detected height : " << boxRect.height <<
				" at (" << boxRect.x << ", " << boxRect.y << ")" << std::endl;
		}

		return pass;
	}

	bool TinEye::textContrastCheck(Image& image, std::vector<Textbox>& boxes) {
		PROFILE_FUNCTION();

		cv::Mat openCVMat = image.getImageMatrix();
		cv::Mat luminanceMap = image.getLuminanceMap();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();

#ifdef _DEBUG
		//Regions of interest
		cv::Mat ROIs = image.getImageMatrix().clone();
		int counter = 0;
#endif // _DEBUG


		if (openCVMat.empty())
		{
			return false;
		}

		bool imagePasses = true;

		for (Textbox box : boxes) {

			box.setParentImage(&image);

			bool individualPass = textboxContrastCheck(box,image);


#ifdef _DEBUG
			if (appSettings->saveTexboxOutline()) {
				cv::Rect boxRect = box.getRect();
				Image::highlightBox(boxRect.x, boxRect.y, boxRect.x + boxRect.width, boxRect.y + boxRect.height, (individualPass) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), ROIs, 2);
			}
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

#ifdef _DEBUG
		if (appSettings->saveTexboxOutline()) {
			image.saveOutputData(ROIs, "contrastChecks.png");
		}
#endif
		return imagePasses;

	}

	bool TinEye::textboxContrastCheck(const Textbox& box, Image& image) {
		PROFILE_FUNCTION();
		cv::Rect boxRect = box.getRect();

		//Contrast checking with thresholds
		cv::Mat luminanceRegion = box.getLuminanceMap();
		cv::Mat maskA,maskB;
		//OTSU threshold automatically calculates best fitting threshold values
		cv::threshold(luminanceRegion, maskA, 30, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
		cv::bitwise_not(maskA, maskB);

		//image.saveOutputData(luminanceRegion, "lum.png");
		//image.saveOutputData(mask, "mask.png");

		double ratio = ContrastBetweenRegions(luminanceRegion, maskA, maskB);

		bool boxPasses = ratio >= config->getGuideline()->getContrastRequirement();

		if (!boxPasses) {
			BOOST_LOG_TRIVIAL(info) << "Word: " << boxRect << " doesn't comply with minimum luminance contrast " << config->getGuideline()->getContrastRequirement()
				<< ", detected contrast ratio is " << ratio << " at: " << boxRect << std::endl;
		}

		return boxPasses;
	}

	double TinEye::ContrastBetweenRegions(const cv::Mat& luminanceMap, const cv::Mat& maskA, const cv::Mat& maskB) {
		//Calculate the mean of the luminance for the light regions of the luminance
		double meanLight = Image::LuminanceMeanWithMask(luminanceMap, maskA);

		//Invert mask to calculate mean of the darker colors
		double meanDark = Image::LuminanceMeanWithMask(luminanceMap, maskB);

		return (std::max(meanLight,meanDark) + 0.05) / (std::min(meanLight,meanDark) + 0.05);
	}

	std::vector<Textbox> TinEye::getTextBoxes(Image& image) {
		PROFILE_FUNCTION();
		return TextboxDetection::detectBoxes(image.getImageMatrix(), config->getAppSettings(), config->getTextDetectionParams());
	}

	void TinEye::mergeTextBoxes(std::vector<Textbox>& textBoxes) {
		PROFILE_FUNCTION();
		std::pair<float, float> threshold = config->getTextDetectionParams()->getMergeThreshold();
		if (threshold.first < 1.0 || threshold.second < 1.0) {
			TextboxDetection::mergeTextBoxes(textBoxes, config->getTextDetectionParams());
		}
	}

	TinEye::~TinEye()
	{
		TextboxDetection::release();

		if (config != nullptr) {
			delete config;
		}

		config = nullptr;
		Instrumentor::Get().EndSession();
	}
}
