#include "TinEye.h"
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>
#include "Configuration.h"
#include "TextboxDetection.h"
#include "boost/log/trivial.hpp"
#include <boost/log/utility/setup/console.hpp>
#include <tesseract/baseapi.h>


void TinEye::init(fs::path configFile)
{
	config = Configuration(configFile.c_str());

	if (api != nullptr)
		delete api;
	api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with language specified by config

	if (api->Init(config.getTessdataPath().string().c_str(), config.getLanguage().c_str())) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	//Manually set dpi 
	api->SetVariable("user_defined_dpi", "70");

	//boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%Severity%] %Message%");
}

bool TinEye::fontSizeCheck(Image& img, std::vector<std::vector<cv::Point>>& boxes) {
	// Set API page segmentation mode to single word, since we know the boxes of the words we'll be searching for
	api->SetPageSegMode(tesseract::PSM_SINGLE_WORD);

	cv::Mat openCVMat = img.getLuminanceMap();
	img.flipLuminance();

	if (openCVMat.empty())
	{
		return false;
	}

	config.setActiveResolution(openCVMat.rows);

	bool passes = true;
	int minimumHeight = config.getHeightRequirement(), minimumWidth = config.getWidthRequirement();
	int padding = 5;

#ifdef _DEBUG
	//Regions of interest
	cv::Mat ROIs = img.getImageMatrix().clone();
	//int counter = 0;
#endif

	for (std::vector<cv::Point> box : boxes) {
		//Set word detection to word bounding box, automatically recognizes
		cv::Rect boxRect(box[1].x - padding, box[1].y - padding, box[2].x - box[1].x + (padding * 2), box[0].y - box[1].y + (padding * 2));
		cv::Mat subMatrix = openCVMat(boxRect);
		api->SetImage(subMatrix.data, subMatrix.cols, subMatrix.rows, 1, subMatrix.step);
		api->Recognize(0);
#ifdef _DEBUG
		//highlight on debug
		Image::highlightBox(boxRect.x, boxRect.y, boxRect.x + boxRect.width, boxRect.y + boxRect.height, cv::Scalar(255, 255, 0), ROIs);
		//cv::imwrite(img.getPath().replace_filename("img" + std::to_string(counter) + ".png").string(), subMatrix);
		//counter++;

#endif
		//int conf = api->MeanTextConf();
		//BOOST_LOG_TRIVIAL(trace) << conf << "\n";
		//if (conf < 80) {
		//	BOOST_LOG_TRIVIAL(info) << "Flipping luminance\n";
		//	//East detected text but tesseract didn't. flip luminance to improve detection and recalculate
		//	img.flipLuminance(box[1].x - padding, box[1].y - padding, box[3].x + padding, box[3].y + padding);
		//	//api->SetImage(openCVMat.data, openCVMat.cols, openCVMat.rows, 1, openCVMat.step);
		//	api->SetImage(subMatrix.data, subMatrix.cols, subMatrix.rows, 1, subMatrix.step);
		//	api->Recognize(0);
		//}

		//conf = api->MeanTextConf();
		//BOOST_LOG_TRIVIAL(trace) << conf << "\n";

		tesseract::ResultIterator* ri = api->GetIterator();
		tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

		if (ri != 0) {
			do {
				if (ri->Confidence(level) >= 80) {
					//Count number of characters in word
					const char* word = ri->GetUTF8Text(level);
					int numberOfChars = strlen(word);
					//BOOST_LOG_TRIVIAL(trace) << word << "\n";

					int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
					ri->BoundingBox(level, &x1, &y1, &x2, &y2);

					//Check average width
					if ((x2 - x1) / numberOfChars < minimumWidth) {
						passes = false;
						//BOOST_LOG_TRIVIAL(info) << "Average character width for word: " << word << " doesn't comply with minimum width, detected width: " << (x2 - x1) / numberOfChars <<
						//	" at (" << x1 << ", " << y1 << ")" << std::endl;
					}

					//Check height
					if (y2 - y1 < minimumHeight) {
						passes = false;
						//BOOST_LOG_TRIVIAL(info) << "Word: '" << word << "' doesn't comply with minimum height " << minimumHeight << ", detected height : " << y2 - y1 <<
						//	" at (" << x1 << ", " << y1 << ")" << std::endl;
					}

					//Check for luminance with background using retrieved bounding box
					int averageBgLuminance = img.getAverageSurroundingLuminance(x1, y1, x2, y2);
					//BOOST_LOG_TRIVIAL(info) << "Average background luminance for line: '" << word << "' is " << averageBgLuminance << std::endl;

					delete[] word;
				}
				else {
					//BOOST_LOG_TRIVIAL(warning) << "Not enough confidence at: " << box[1] << " " << box[3] << std::endl;
				}
			} while (ri->Next(level));
		}
		else {
			//BOOST_LOG_TRIVIAL(warning) << "Tesseract can't detect text in region" << box[1] << " " << box[3] << std::endl;
		}



		delete ri;
	}
#ifdef _DEBUG
	cv::imwrite(img.getPath().replace_filename(img.getPath().stem().string() + "_inputBoxes.png").string(), ROIs);
#endif

	//BOOST_LOG_TRIVIAL(info) << ((passes) ? "PASS" : "FAIL") << std::endl;

	//pixDestroy(&image);

	return passes;
}

bool TinEye::fontSizeCheck(Image& img) {
	// Set API page segmentation mode to sparse text (finds as much text as possible in no particular order)
	api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);

	cv::Mat openCVMat = img.getLuminanceMap();

	if (openCVMat.empty())
	{
		return false;
	}


	api->SetImage(openCVMat.data, openCVMat.cols, openCVMat.rows, 1, openCVMat.step);

	api->Recognize(0);

	config.setActiveResolution(openCVMat.rows);

	//Start font size test
	bool passes = true;
	int minimumHeight = config.getHeightRequirement(), minimumWidth = config.getWidthRequirement();


	//Test for width of each individual character
	//Can recognize words, letters, lines or complete blocks
	tesseract::ResultIterator* ri = api->GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			int x1, y1, x2, y2;
			float conf = ri->Confidence(level);
			if (conf >= 80) {
				//BOOST_LOG_TRIVIAL(trace) << "confidence: " << conf << " ";
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				//BOOST_LOG_TRIVIAL(trace) << "height: " << y2 - y1 << " ";
				//BOOST_LOG_TRIVIAL(trace) << "width: " << x2 - x1 << " ";
				//BOOST_LOG_TRIVIAL(trace) << "line: " << word << std::endl;
				if (x2 - x1 < minimumWidth) {
					passes = false;
					//BOOST_LOG_TRIVIAL(info) << "Character " << word << " doesn't comply with minimum width, detected width: " << x2 - x1 <<
					//	" at (" << x1 << ", " << y1 << ")" << std::endl;
				}
			}

			delete[] word;
		} while (ri->Next(level) && passes);
	}

	delete ri;
	//Test for height for entire lines (so it includes characters that are longer and shorter (trying to approximate hp distance)
	ri = api->GetIterator();
	level = tesseract::RIL_WORD;
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			int x1, y1, x2, y2;
			float conf = ri->Confidence(level);
			if (conf >= 50) {
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				if (y2 - y1 < minimumHeight) {
					//passes = false;
					//BOOST_LOG_TRIVIAL(info) << "Line: '" << word << "' doesn't comply with minimum height " << minimumHeight << ", detected height : " << y2 - y1 <<
					//	" at (" << x1 << ", " << y1 << ")" << std::endl;
				}

				//Check for luminance with background using retrieved bounding box
				int averageBgLuminance = img.getAverageSurroundingLuminance(x1, y1, x2, y2);
				//BOOST_LOG_TRIVIAL(info) << "Average background luminance for line: '" << word << "' is " << averageBgLuminance << std::endl;
			}

			delete[] word;
		} while (ri->Next(level) && passes);
	}

	delete ri;

	return passes;
}

bool TinEye::fontSizeCheck(fs::path imagePath, bool EASTBoxing)
{
	//Open input image with openCV
	Image img;
	img.loadImage(imagePath);

	bool testResult = false;

	do {
		if (EASTBoxing) {
			//BOOST_LOG_TRIVIAL(debug) << "Using EAST preprocessing" << std::endl;
			//Check if image has text recognized by OCR
			std::vector<std::vector<cv::Point>> textBoxes = TextboxDetection::detectBoxes(img.getImageMatrix(), false);

			if (textBoxes.empty()) {
				//BOOST_LOG_TRIVIAL(info) << "No words recognized in image" << std::endl;
			}
			else {
				// Get OCR result
				testResult = fontSizeCheck(img, textBoxes);
			}

		}
		else {
			testResult = fontSizeCheck(img);
		}
	} while (img.nextFrame());


	return testResult;

}

TinEye::~TinEye()
{
	if (api != nullptr)
	{
		api->End();
		delete api;
	}

	api = nullptr;
}
