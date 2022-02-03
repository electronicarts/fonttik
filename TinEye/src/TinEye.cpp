// TinEye.cpp : Defines the entry point for the application.
//

#include "TinEye.h"
#include "Configuration.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>
#include "Image.h"

void GenerateLuminanceMap()
{
	std::cout << "Enter image path:";
	std::string fileName;
	std::getline(std::cin, fileName);

	Image img;
	img.loadImage(fileName);
	img.getLuminanceMap();
	img.saveLuminanceMap(fileName + "_luminance.png");
}



int main(int argc, char* argv[]) {
	GenerateLuminanceMap();
	
	Configuration config("config.json");

	config.setActiveResolution((argc > 1) ? atoi(argv[1]) : 1080);

	std::cout << config.getHeightRequirement() << "\t" << config.getContrastRequirement() << std::endl;

	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init("tessdata/",config.getLanguage().c_str())) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// Open input image with leptonica library
	Pix* image = pixRead("resources/sizes/2160p-47x94_47x94_47x94_47x94_47x94.png");
	api->SetImage(image);
	api->Recognize(0);
	// Get OCR result
	/*outText = api->GetUTF8Text();
	printf("OCR output:\n%s", outText);*/

	tesseract::ResultIterator* ri = api->GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL; //Mirar con symbol?
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			int x1, y1, x2, y2;
			float conf = ri->Confidence(level);
			if (conf >= 80) {
				std::cout << "confidence: " << conf << " ";
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				std::cout << "height: " << y2 - y1 << " ";
				std::cout << "width: " << x2 - x1 << " ";
				std::cout << "char: " << word << std::endl;
			}

			delete[] word;
		} while (ri->Next(level));
	}

	// Destroy used object and release memory
	api->End();
	delete api;
	//delete[] outText;
	pixDestroy(&image);
	std::cin.get();
	return 0;
}
