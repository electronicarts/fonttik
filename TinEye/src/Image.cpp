#include "Image.h"

Image::Image() {

}

bool Image::loadImage(std::string filepath) {
	//IMREAD_COLOR transforms image to BGR format
	imageMatrix = cv::imread(filepath, cv::IMREAD_COLOR);

	if (imageMatrix.empty()) {
		std::cout << "Could not read image: " << filepath << std::endl;
		return false;
	}
	else {
		return true;
	}
}

cv::Mat Image::getLuminanceMap() {
	if (!imageMatrix.empty()) {
		//Load pixel information to float matrix (so bgr values go from 0.0 to 1.0)
		cv::Mat linearBGR = cv::Mat::zeros(imageMatrix.size(), CV_32FC3); //1 channel (luminance)

		for (int y = 0; y < imageMatrix.rows; y++) {
			for (int x = 0; x < imageMatrix.cols; x++) {
				cv::Vec3b colorVals = imageMatrix.at<cv::Vec3b>(y, x);

				//TODO lookup table, inexpensive, only 256 values, one for each lum value
				//Could also use three separate lookup tables and merge them into one result directly
				linearBGR.at<cv::Vec3f>(y, x) = {
					linearize8bitRGB(colorVals.val[0]),
					linearize8bitRGB(colorVals.val[1]),
					linearize8bitRGB(colorVals.val[2]) };
			}
		}


		luminanceMap = cv::Mat::zeros(imageMatrix.size(), CV_8UC1); //1 channel (luminance)

		for (int y = 0; y < imageMatrix.rows; y++) {
			for (int x = 0; x < imageMatrix.cols; x++) {
				cv::Vec3f lumVals = linearBGR.at<cv::Vec3f>(y, x);
				//BGR order
				luminanceMap.at<uchar>(y, x) = cv::saturate_cast<uchar>((lumVals.val[0] * 0.0722 + lumVals.val[1] * 0.7152 + lumVals.val[2] * 0.2126) * 255);
			}
		}
	}

	return luminanceMap;
}

void Image::saveLuminanceMap(std::string filepath) {
	//Only save if we've already generated the image's luminance map
	if (!luminanceMap.empty()) {
		//Save image to same filepath
		cv::imwrite(filepath, luminanceMap);
	}
}

float Image::linearize8bitRGB(const uchar& colorBits) {
	//ref https://developer.mozilla.org/en-US/docs/Web/Accessibility/Understanding_Colors_and_Luminance
	float color = colorBits / 255.0f;

	if (color <= 0.04045) {
		return color / 12.92f;
	}
	else {
		return pow((color + 0.055f) / 1.1055f, 2.4f);
	}
}