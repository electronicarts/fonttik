// TinEye.cpp : Defines the entry point for the application.
//

#include "TinEye.h"
#include "Configuration.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>
#include "Image.h"

Image GenerateLuminanceMap()
{
	std::cout << "Enter image path:";
	std::string fileName;
	std::getline(std::cin, fileName);

	Image img;
	img.loadImage(fileName);
	img.getLuminanceMap();
	img.saveLuminanceMap(fileName + "_luminance.png");
	return img;
}

bool fontSizeCheck(Image& img, Configuration& config, tesseract::TessBaseAPI* api) {
	// Open input image with leptonica library
	//Pix* image = pixRead("resources/sizes/1080p-31x40.png");
	//api->SetImage(image);

	//Open input image with openCV
	cv::Mat openCVMat = img.getLuminanceMap();

	if (openCVMat.empty())
		return false;

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
						" at (" <<x1 << ", " << y1 <<")" << std::endl;
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
					passes = false;
					std::cout << "Line: '" << word << "' doesn't comply with minimum height " << minimumHeight << ", detected height : " << y2 - y1 << 
						" at (" << x1 << ", " << y1 << ")" << std::endl;
				}

				//Check for luminance with background using retrieved bounding box
				int averageBgLuminance = img.getAverageSurroundingLuminance(x1, x2, y1, y2);
				std::cout << "Average background luminance for line: '" << word << "' is " << averageBgLuminance << std::endl;
			}

			delete[] word;
		} while (ri->Next(level) && passes);
	}
	delete ri;

	std::cout << ((passes) ? "PASS" : "FAIL") << std::endl;

	//pixDestroy(&image);

	return passes;
}

int main(int argc, char* argv[]) {
	Configuration config("config.json");

	//config.setActiveResolution((argc > 1) ? atoi(argv[1]) : 1080);

	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with language specified by config
	if (api->Init("tessdata/", config.getLanguage().c_str())) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	Image img = GenerateLuminanceMap();

	// Get OCR result

	fontSizeCheck(img, config, api);

	// Destroy used object and release memory
	api->End();
	delete api;
	//delete[] outText;
	std::cin.get();
	return 0;
}
