// TinEye.cpp : Defines the entry point for the application.
//

#include "TinEye.h"
#include "Configuration.h"
#include "TextboxDetection.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>
#include "Image.h"
#include <filesystem>

Image GenerateLuminanceMap()
{
	std::cout << "Enter image path:";
	std::string fileName;
	std::getline(std::cin, fileName);

	Image img;
	img.loadImage(fileName);
	img.getLuminanceMap();
	//img.flipLuminance(0, 0, 1260, 230);
	img.saveLuminanceMap(fileName + "_luminance.png");
	return img;
}

bool fontSizeCheck(Image& img, Configuration& config, tesseract::TessBaseAPI* api, std::vector<std::vector<cv::Point>>& boxes) {
	// Set API page segmentation mode to single word, since we know the boxes of the words we'll be searching for
	api->SetPageSegMode(tesseract::PSM_SINGLE_WORD);

	cv::Mat openCVMat = img.getLuminanceMap();

	if (openCVMat.empty())
	{
		return false;
	}

	config.setActiveResolution(openCVMat.rows);

	bool passes = true;
	int minimumHeight = config.getHeightRequirement(), minimumWidth = config.getWidthRequirement();
	int padding = 10;

	for (std::vector<cv::Point> box : boxes) {
		//Set word detection to word bounding box, automatically recognizes
		cv::Mat subMatrix = openCVMat(cv::Rect(box[1].x - padding, box[1].y - padding, box[2].x - box[1].x + padding, box[0].y - box[1].y + padding));
		api->SetImage(subMatrix.data, subMatrix.cols, subMatrix.rows, 1, subMatrix.step);
		api->Recognize(0);

		//int conf = api->MeanTextConf();
		//std::cout << conf << "\n";
		//if (conf < 80) {
		//	std::cout << "Flipping luminance\n";
		//	//East detected text but tesseract didn't. flip luminance to improve detection and recalculate
		//	img.flipLuminance(box[1].x - padding, box[1].y - padding, box[3].x + padding, box[3].y + padding);
		//	//api->SetImage(openCVMat.data, openCVMat.cols, openCVMat.rows, 1, openCVMat.step);
		//	api->SetImage(subMatrix.data, subMatrix.cols, subMatrix.rows, 1, subMatrix.step);
		//	api->Recognize(0);
		//}

		//conf = api->MeanTextConf();
		//std::cout << conf << "\n";

		tesseract::ResultIterator* ri = api->GetIterator();
		tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

		if (ri != 0) {
			do {
				if (ri->Confidence(level) >= 80) {
					//Count number of characters in word
					const char* word = ri->GetUTF8Text(level);
					int numberOfChars = strlen(word);
					//std::cout << word << "\n";

					int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
					ri->BoundingBox(level, &x1, &y1, &x2, &y2);

					//Check average width
					if ((x2 - x1) / numberOfChars < minimumWidth) {
						passes = false;
						std::cout << "Average character width for word: " << word << " doesn't comply with minimum width, detected width: " << (x2 - x1) / numberOfChars <<
							" at (" << x1 << ", " << y1 << ")" << std::endl;
					}

					//Check height
					if (y2 - y1 < minimumHeight) {
						passes = false;
						std::cout << "Word: '" << word << "' doesn't comply with minimum height " << minimumHeight << ", detected height : " << y2 - y1 <<
							" at (" << x1 << ", " << y1 << ")" << std::endl;
					}

					//Check for luminance with background using retrieved bounding box
					int averageBgLuminance = img.getAverageSurroundingLuminance(x1, y1, x2, y2);
					std::cout << "Average background luminance for line: '" << word << "' is " << averageBgLuminance << std::endl;

					delete[] word;
				}
				else {
					std::cout << "Not enough confidence at: " << box[1] << " " << box[3] << std::endl;
				}
			} while (ri->Next(level));
		}
		else {
			std::cout << "Tesseract can't detect text in region " << box[1] << " " << box[3] << std::endl;
		}



		delete ri;
	}

	std::cout << ((passes) ? "PASS" : "FAIL") << std::endl;

	//pixDestroy(&image);

	return passes;
}

bool fontSizeCheck(Image& img, Configuration& config, tesseract::TessBaseAPI* api) {
	// Set API page segmentation mode to sparse text (finds as much text as possible in no particular order)
	api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
	// Open input image with leptonica library
	//Pix* image = pixRead("resources/sizes/1080p-31x40.png");
	//api->SetImage(image);

	//Open input image with openCV
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
				//std::cout << "confidence: " << conf << " ";
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				//std::cout << "height: " << y2 - y1 << " ";
				//std::cout << "width: " << x2 - x1 << " ";
				//std::cout << "line: " << word << std::endl;
				if (x2 - x1 < minimumWidth) {
					passes = false;
					std::cout << "Character " << word << " doesn't comply with minimum width, detected width: " << x2 - x1 <<
						" at (" << x1 << ", " << y1 << ")" << std::endl;
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
			if (conf >= 80) {
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				if (y2 - y1 < minimumHeight) {
					//passes = false;
					std::cout << "Line: '" << word << "' doesn't comply with minimum height " << minimumHeight << ", detected height : " << y2 - y1 <<
						" at (" << x1 << ", " << y1 << ")" << std::endl;
				}

				//Check for luminance with background using retrieved bounding box
				int averageBgLuminance = img.getAverageSurroundingLuminance(x1, y1, x2, y2);
				std::cout << "Average background luminance for line: '" << word << "' is " << averageBgLuminance << std::endl;
			}

			delete[] word;
		} while (ri->Next(level) && passes);
	}

	delete ri;

	ri = api->GetIterator();
	level = tesseract::RIL_WORD;
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			int x1, y1, x2, y2;
			float conf = ri->Confidence(level);
			if (conf >= 80) {
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				std::cout << "Line: '" << word << "' at (" << x1 << ", " << y1 << ")" << std::endl;

				//Check for luminance with background using retrieved bounding box
				int averageBgLuminance = img.getAverageSurroundingLuminance(x1, y1, x2, y2);
				std::cout << "Average background luminance for line: '" << word << "' is " << averageBgLuminance << std::endl;
			}

			delete[] word;
		} while (ri->Next(level));
	}
	delete ri;

	std::cout << ((passes) ? "PASS" : "FAIL") << std::endl;

	//pixDestroy(&image);

	return passes;
}

int main(int argc, char* argv[]) {
	Configuration config("config.json");


	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with language specified by config

	if (api->Init(config.getTessdataPath().string().c_str(), config.getLanguage().c_str())) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	std::cout << "Enter image path:";
	std::string fileName;
	std::getline(std::cin, fileName);

	Image img;
	img.loadImage(fileName);

	////Check if image has text recognized by OCR
	//std::vector<std::vector<cv::Point>> textBoxes = TextboxDetection::detectBoxes(img.getImageMatrix(), true);

	//if (textBoxes.empty()) {
	//	std::cout << "No words recognized in image" << std::endl;
	//}
	//else {
	//	// Get OCR result
	//	fontSizeCheck(img, config, api, textBoxes);
	//}

	fontSizeCheck(img, config, api);


	// Destroy used object and release memory
	api->End();
	delete api;
	//delete[] outText;
	std::cin.get();
	return 0;
}
