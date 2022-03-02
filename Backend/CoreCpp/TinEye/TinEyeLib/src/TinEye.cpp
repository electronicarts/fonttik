#include "TinEye.h"
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>
#include "Configuration.h"
#include "Image.h"
#include "TextboxDetection.h"
#include "boost/log/trivial.hpp"
#include <tesseract/baseapi.h>
#include "Guideline.h"
#include "AppSettings.h"

void TinEye::init(fs::path configFile)
{
	config = new Configuration(configFile.c_str());

	if (api != nullptr)
		delete api;
	api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with language specified by config

	if (api->Init("tessdata/", "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	//Manually set dpi 
	api->SetVariable("user_defined_dpi", "70");
}

void TinEye::applyFocusMask(Image& image) {
	cv::Mat img = image.getImageMatrix();
	cv::Mat mask = config->getAppSettings()->calculateMask(img.cols,img.rows);
	img = img & mask;
}

bool TinEye::fontSizeCheck(Image& img, std::vector<Textbox>& boxes) {
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
	int minimumHeight = guideline->getHeightRequirement(), minimumWidth = guideline->getWidthRequirement();
	int padding = 0;

#ifdef _DEBUG
	//Regions of interest
	cv::Mat ROIs = img.getImageMatrix().clone();
	int counter = 0;
#endif

	//from https://docs.opencv.org/4.x/d4/d43/tutorial_dnn_text_spotting.html

	// Load models weights
	cv::dnn::TextRecognitionModel model("crnn_cs.onnx");
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


	for (Textbox box : boxes) {
		bool individualPass = true;
		//Set word detection to word bounding box
		box.setParentImage(&img);
		cv::Rect boxRect = box.getRect();

		//Recognize word in region
		std::string recognitionResult = model.recognize(box.getSubmatrix());

		//Check average width
		if (boxRect.width / recognitionResult.size() < minimumWidth) {
			passes = false;
			individualPass = false;
			BOOST_LOG_TRIVIAL(info) << "Average character width for word: " << recognitionResult << " doesn't comply with minimum width, detected width: " << boxRect.width / recognitionResult.size() <<
				" at (" << boxRect.x << ", " << boxRect.y << ")" << std::endl;
		}


		//Check height
		if (boxRect.height < minimumHeight) {
			passes = false;
			individualPass = false;
			BOOST_LOG_TRIVIAL(info) << "Word: '" << recognitionResult << "' doesn't comply with minimum height " << minimumHeight << ", detected height : " << boxRect.height <<
				" at (" << boxRect.x << ", " << boxRect.y << ")" << std::endl;
		}

#ifdef _DEBUG
		if (appSettings->saveTexboxOutline()) {
			Image::highlightBox(boxRect.x, boxRect.y, boxRect.x + boxRect.width, boxRect.y + boxRect.height, (individualPass) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), ROIs, 2);
		}
		if (appSettings->saveSeparateTextboxes()) {
			cv::imwrite(img.getPath().replace_filename("img" + std::to_string(counter) + ".png").string(), box.getSubmatrix());
		}
		if (appSettings->saveHistograms()) {
			fs::path savePath = img.getPath().replace_filename("img" + std::to_string(counter) + "histogram.png").string();
			Image::saveLuminanceHistogram(box.getLuminanceHistogram(),
				savePath.string());

			Image::saveHistogramCSV(img.calculateLuminanceHistogram(boxRect), img.getPath().replace_filename("histogram" + std::to_string(counter) + ".csv").string());
		}
		counter++;

#endif

		//Contrast checking with thresholds
		cv::Mat luminanceRegion = luminanceMap(boxRect);
		cv::Mat mask;
		//OTSU threshold automatically calculates best fitting threshold values
		cv::threshold(luminanceRegion, mask, 30, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

		//Calculate the mean of the luminance for the light regions of the luminance
		double meanLight = cv::mean(luminanceRegion, mask)[0]/255.0;

		//Invert mask to calculate mean of the darker colors
		cv::bitwise_not(mask, mask);
		double meanDark = cv::mean(luminanceRegion, mask)[0]/255.0;

		double ratio;
		if (meanLight >= meanDark) {
			ratio = (meanLight + .05) / (meanDark + .05);
		}
		else {
			ratio = (meanDark + .05) / (meanLight + .05);
		}

		if (ratio < guideline->getContrastRequirement()) {
			BOOST_LOG_TRIVIAL(info) << "Word: " << recognitionResult << " doesn't comply with minimum luminance contrast " << guideline->getContrastRequirement()
				<< ", detected contrast ratio is " << ratio << " at: " << boxRect << std::endl;
		}

		//cv::imshow("test", mask);
		//cv::waitKey();
	}
#ifdef _DEBUG
	if (appSettings->saveTexboxOutline()) {
		cv::imwrite(img.getPath().replace_filename(img.getPath().stem().string() + "_inputBoxes.png").string(), ROIs);
	}
#endif

	BOOST_LOG_TRIVIAL(info) << ((passes) ? "PASS" : "FAIL") << std::endl;

	//pixDestroy(&image);

	return passes;
}

bool TinEye::textContrastCheck(Image& image, std::vector<Textbox>& boxes) {
	bool imagePasses = true;

	for (Textbox box : boxes) {
		cv::Rect boxRect = box.getRect();
		//Check for luminance with background using retrieved bounding box
		int averageBgLuminance = image.getAverageSurroundingLuminance(boxRect);
		//TODO get luminance of text
		//TODO output test result
		bool boxPasses = true;
		//TODO highlight box in function of boxpasses
		imagePasses = imagePasses && boxPasses;
	}

	return imagePasses;
}

std::vector<Textbox> TinEye::getTextBoxes(Image& image) {
	return TextboxDetection::detectBoxes(image.getImageMatrix(), config->getAppSettings(), config->getTextDetectionParams());
}

TinEye::~TinEye()
{
	if (api != nullptr)
	{
		api->End();
		delete api;
	}

	if (config != nullptr) {
		delete config;
	}

	config = nullptr;
	api = nullptr;
}
