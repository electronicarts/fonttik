#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <filesystem>

class Image {
protected:
	cv::Mat imageMatrix;
	cv::Mat luminanceMap;
	std::filesystem::path path;

	cv::VideoCapture video;

	bool isVideo = false;
	std::vector<std::string > imageFormats;

	void convertImageMatrixToBGR();

	static float linearize8bitRGB(const uchar& colorBits);

	static cv::Mat generateLuminanceHistogramImage(cv::Mat histogram);
public:
	Image();

	bool loadImage(std::filesystem::path filepath);

	//Returns loaded image matrix
	cv::Mat getImageMatrix();

	//Returns loaded image's luminance map, if map hasn't been calculated calculates it as well
	cv::Mat getLuminanceMap();

	//Saves calculated luminance map to specified filepath
	void saveLuminanceMap(std::string filepath);

	//Flips the luminance of a given region
	void flipLuminance(const int& x1, const int& y1, const int& x2, const int& y2);

	//Flips the luminance of the whole image
	void flipLuminance();

	//Returns average surrounding luminance of a given bounding box in luminance map
	uchar getAverageSurroundingLuminance(const int& x1, const int& y1, const int& x2, const int& y2, const int& marginX = 3, const int& marginY = 3);

	//Returns average surrounding luminance of a given bounding box in luminance map
	uchar getAverageSurroundingLuminance(cv::Rect region, const int& marginX = 3, const int& marginY = 3);

	//Hightlights box in specified matrix
	static void highlightBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& matrix, int thickness = 1);

	//Calculates the luminance histogram of a region
	cv::Mat calculateLuminanceHistogram(cv::Rect, cv::Rect ignoreRegion=cv::Rect(0,0,0,0));
	
	//Calculates the luminance histogram of an image
	cv::Mat calculateLuminanceHistogram();

	static void displayLuminanceHistogram(cv::Mat histogram);

	static void saveLuminanceHistogram(cv::Mat histogram, std::string filepath);

	//If loaded file is a video grabs the next frame and returns true, if no frame available or file is an image returns false
	bool nextFrame();

	//Path of the original image or the video its coming from
	std::filesystem::path getPath() { return path; };
};