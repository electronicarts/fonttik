// TinEye.cpp : Defines the entry point for the application.
//

#include "TinEye.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>

int main(int argc, char* argv[]) {
	char* outText;

	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init("tessdata/", "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// Open input image with leptonica library
	Pix* image = pixRead("resources/gradiente.png");
	api->SetImage(image);
	api->Recognize(0);
	// Get OCR result
	/*outText = api->GetUTF8Text();
	printf("OCR output:\n%s", outText);*/

	tesseract::ResultIterator* ri = api->GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD; //Mirar con symbol?
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			int x1, y1, x2, y2;
			float conf = ri->Confidence(level);
			if (conf >= 80) {
				std::cout << "confidence: " << conf << " ";
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				std::cout << "height: " << y2 - y1 << " ";
				std::cout << "word: " << word << std::endl;
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
	return 3;
}
