#include "Image.h"
#include <boost/log/trivial.hpp>


void Image::highlightBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& matrix, int thickness)
{
	if (!matrix.empty()) {
		cv::line(matrix, cv::Point(x1, y1), cv::Point(x2, y1), color, thickness);
		cv::line(matrix, cv::Point(x2, y1), cv::Point(x2, y2), color, thickness);
		cv::line(matrix, cv::Point(x2, y2), cv::Point(x1, y2), color, thickness);
		cv::line(matrix, cv::Point(x1, y2), cv::Point(x1, y1), color, thickness);
	}
}

cv::Mat Image::calculateLuminanceHistogram(const int& x1, const int& y1, const int& x2, const int& y2)
{
	cv::Mat region = luminanceMap(cv::Rect(x1, y1, x2 - x1, y2 - y1));
	cv::Mat histogram;

	if (!luminanceMap.empty()) {
		int histSize = 256;
		float range[] = { 0,256 };
		const float* histRange[] = { range };

		cv::calcHist(&region, 1, 0, cv::Mat(), histogram, 1, &histSize, histRange, true, false);
	}

	return histogram;
}

cv::Mat Image::calculateLuminanceHistogram()
{
	return calculateLuminanceHistogram(0, 0, luminanceMap.cols - 1, luminanceMap.rows - 1);
}

void Image::displayLuminanceHistogram(cv::Mat histogram)
{
	cv::Mat histImage = generateLuminanceHistogramImage(histogram);
	cv::imshow("histogram", histImage);
	cv::waitKey();
}

void Image::saveLuminanceHistogram(cv::Mat histogram, std::string filepath)
{
	cv::Mat histImage = generateLuminanceHistogramImage(histogram);
	cv::imwrite(filepath, histImage);
}

bool Image::nextFrame()
{
	imageMatrix.release();
	luminanceMap.release();

	if (isVideo) {
		video >> imageMatrix;
		if (imageMatrix.empty()) {
			BOOST_LOG_TRIVIAL(info) << "No more images" << std::endl;
			video.release();
		}
		else {
			return true;
		}
	}

	return false;
}

Image::Image() {
	imageFormats = { ".bmp",".jpeg",".jpg",".jpe",".jp2",".png",".webp",".pbm",".pgm",".ppm",".pnm",".sr",".ras",".tiff",".tif",".exr",".hdr",".pic" };
}

bool Image::loadImage(std::filesystem::path filepath) {
	path = filepath;

	std::string fileFormat = filepath.extension().string();

	//Check if given file is an admitted image type, if not, attempt to load as video
	if (std::find(imageFormats.begin(), imageFormats.end(), fileFormat) != imageFormats.end()) {
		isVideo = false;

		imageMatrix = cv::imread(filepath.string(), cv::IMREAD_COLOR);
	}
	else {
		isVideo = true;
		video = cv::VideoCapture(filepath.string());

		if (video.isOpened()) {
			video >> imageMatrix;
		}
	}

	//If imagematrix is empty something has gone wrong when loading
	if (imageMatrix.empty()) {
		BOOST_LOG_TRIVIAL(error) << "Could not read image: " << filepath << std::endl;
	}
	else {
		convertImageMatrixToBGR();
		return true;
	}

	return false;
}

cv::Mat Image::getImageMatrix()
{
	return imageMatrix;
}

cv::Mat Image::getLuminanceMap() {
	//Make sure that image has been loaded and we haven't previously calculated the luminance already
	if (!imageMatrix.empty() && luminanceMap.empty()) {
		//Load pixel information to float matrix (so bgr values go from 0.0 to 1.0)

		//Matrix to store linearized rgb
		cv::Mat linearBGR = cv::Mat::zeros(imageMatrix.size(), CV_32FC3);

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

void Image::flipLuminance(const int& x1, const int& y1, const int& x2, const int& y2)
{
	if (!luminanceMap.empty()) {
		cv::Mat subMatrix = luminanceMap(cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1));
		cv::bitwise_not(subMatrix, subMatrix);
	}
}

void Image::flipLuminance()
{
	if (!luminanceMap.empty()) {
		flipLuminance(0, 0, luminanceMap.cols - 1, luminanceMap.rows - 1);
	}
}

uchar Image::getAverageSurroundingLuminance(const int& x1, const int& y1, const int& x2, const int& y2, const int& marginX, const int& marginY)
{
	uchar lum = 0;

	if (!luminanceMap.empty()) {


		int yMin = std::max(y1 - marginY, 0), yMax = std::min(y2 + marginY, luminanceMap.rows);
		int xMin = std::max(x1 - marginX, 0), xMax = std::min(x2 + marginX, luminanceMap.cols);

		int cumulativeLuminance = 0, valuesChecked = 0;

		for (int y = yMin; y < yMax; y++) {
			for (int x = xMin; x < xMax; x++) {
				//Only use values outside of text bounding box
				if (!(x > x1 && x<x2 && y>y1 && y < y2)) {
					cumulativeLuminance += luminanceMap.at<uchar>(y, x);
					valuesChecked++;
				}
			}
		}

		lum = cumulativeLuminance / valuesChecked;
	}

	return lum;
}

void Image::convertImageMatrixToBGR()
{
	if (imageMatrix.channels() == 1) {
		cv::cvtColor(imageMatrix, imageMatrix, cv::COLOR_GRAY2BGR);
	}
	else if (imageMatrix.channels() == 4) {
		cv::cvtColor(imageMatrix, imageMatrix, cv::COLOR_BGRA2BGR);
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

cv::Mat Image::generateLuminanceHistogramImage(cv::Mat histogram)
{
	//Display the histogram
	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);
	cv::Mat histImage(hist_h, hist_w, CV_8UC1, cv::Scalar(0, 0, 0));

	cv::normalize(histogram, histogram, 0, histImage.rows, cv::NORM_MINMAX);

	for (int i = 1; i < 256; i++) {
		line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(histogram.at<float>(i - 1))), cv::Point(bin_w * (i), hist_h - cvRound(histogram.at<float>(i))), cv::Scalar(255), 1, 8, 0);
	}


	return histImage;
}
