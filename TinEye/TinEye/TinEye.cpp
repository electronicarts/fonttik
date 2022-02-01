// TinEye.cpp : Defines the entry point for the application.
//

#include "TinEye.h"

#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <leptonica/allheaders.h>
#include <iostream>
#include <string>

float linearize8bitRGB(const uchar& colorBits) {
	//ref https://developer.mozilla.org/en-US/docs/Web/Accessibility/Understanding_Colors_and_Luminance
	float color = colorBits / 255.0f;

	if (color <= 0.04045) {
		return color / 12.92f;
	}
	else {
		return pow((color + 0.055f) / 1.1055f, 2.4f);
	}
}

void GenerateLuminanceMap()
{
	std::cout << "Enter image path:";
	std::string fileName;
	std::getline(std::cin, fileName);

	//IMREAD_COLOR transforms image to BGR format
	cv::Mat img = cv::imread(fileName, cv::IMREAD_COLOR);

	if (img.empty()) {
		std::cout << "Could not read image: " << fileName << std::endl;
	}
	else {
		//cv::imshow("Display window", img);
		//Process image

		//Load pixel information to float matrix (so bgr values go from 0.0 to 1.0)
		cv::Mat linearBGR = cv::Mat::zeros(img.size(), CV_32FC3); //1 channel (luminance)

		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				cv::Vec3b colorVals = img.at<cv::Vec3b>(y, x);

				//TODO lookup table, inexpensive, only 256 values, one for each lum value
				//Could also use three separate lookup tables and merge them into one result directly
				linearBGR.at<cv::Vec3f>(y, x) = { 
					linearize8bitRGB(colorVals.val[0]),
					linearize8bitRGB(colorVals.val[1]),
					linearize8bitRGB(colorVals.val[2]) };
			}
		}

		//Export luminance image

		cv::Mat luminanceMap = cv::Mat::zeros(img.size(), CV_8UC1); //1 channel (luminance)

		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				cv::Vec3f lumVals = linearBGR.at<cv::Vec3f>(y, x);
				//BGR order
				luminanceMap.at<uchar>(y, x) = cv::saturate_cast<uchar>((lumVals.val[0] * 0.0722 + lumVals.val[1] * 0.7152 + lumVals.val[2] * 0.2126)*255);
			}
		}

		//Save image to same filepath with suffix
		cv::imwrite(fileName + "luminance.png", luminanceMap);
	}
}

#include "Configuration.h"
#include "nlohmann/json.hpp"

int main(int argc, char* argv[]) {
	GenerateLuminanceMap();
	return 0;
	char* outText;
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
