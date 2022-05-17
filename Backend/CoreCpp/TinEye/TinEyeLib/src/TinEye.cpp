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
#include "ContrastChecker.h"
#include "SizeChecker.h"
#include <limits>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace tin {
	std::vector<double>* TinEye::linearizationLUT = nullptr;

	Results* TinEye::processMedia(Media& media) {
		//Save the logs or output through console
		if (config->getAppSettings()->saveLogs()) {
			if (logSink) {
				logging::core::get()->remove_sink(logSink);
			}
			std::string logFile = media.getOutputPath().string();
			logFile = logFile.substr(0, logFile.length() - 4);
			logSink = boost::log::add_file_log(media.getOutputPath().string() + "/" + media.getPath().stem().string() + ".txt");

			boost::log::core::get()->set_filter
			(
				boost::log::trivial::severity >= boost::log::trivial::info
			);
		}
		else {
			if (logSink) {
				logging::core::get()->remove_sink(logSink);
			}
		}
		do {
			//Check if image has text recognized by OCR
			applyFocusMask(media);
			std::vector<tin::Textbox> textBoxes = getTextBoxes(media);
			mergeTextBoxes(textBoxes);
			if (textBoxes.empty()) {
				BOOST_LOG_TRIVIAL(info) << "No words recognized in image" << std::endl;
			}
			else {
				fontSizeCheck(media, textBoxes);
				textContrastCheck(media, textBoxes);
			}
		} while (media.nextFrame());

		BOOST_LOG_TRIVIAL(info) << "SIZE: " << ((media.getResultsPointer()->overallSizePass) ? "PASS" : "FAIL") <<
			"\tCONTRAST: " << ((media.getResultsPointer()->overallContrastPass) ? "PASS" : "FAIL") << std::endl;

		return media.getResultsPointer();
	}

	void TinEye::init(Configuration* configuration)
	{

		Instrumentor::Get().BeginSession("Profile", "profiling.json");	
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
	
		//Create checkers
		contrastChecker = new ContrastChecker(config);
		sizeChecker = new SizeChecker(config, textboxRecognition);
	}

	void TinEye::applyFocusMask(Media& image) {
		PROFILE_FUNCTION();
		cv::Mat img = image.getImageMatrix();
		cv::Mat mask = config->getAppSettings()->calculateMask(img.cols, img.rows);
		img = img & mask;
	}

	bool TinEye::fontSizeCheck(Media& image, std::vector<Textbox>& boxes) {
		sizeChecker->check(image, boxes);
	}

	bool TinEye::textContrastCheck(Media& image, std::vector<Textbox>& boxes) {
		contrastChecker->check(image, boxes);
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

		if (contrastChecker != nullptr) {
			delete contrastChecker;
		}
		contrastChecker = nullptr;

		if (sizeChecker != nullptr) {
			delete sizeChecker;
		}
		sizeChecker = nullptr;

		config = nullptr;
		Instrumentor::Get().EndSession();
	}
}
