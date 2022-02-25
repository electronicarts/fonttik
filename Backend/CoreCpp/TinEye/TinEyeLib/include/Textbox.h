#pragma once

#include <opencv2/core.hpp>
#include "Image.h"

class Textbox {
protected:
	cv::Rect textboxRect;

	cv::Mat submatrix;
	cv::Mat luminanceHistogram;

	Image* parentImage;
public:
	Textbox(const std::vector<cv::Point >& points, int padding = 0);

	//Sets textbox's parent image and calculates its submatrix
	void setParentImage(Image* img);

	//Returns and saves the luminance histogram of the specified textbox rect
	cv::Mat getLuminanceHistogram();

	//Returns the luminance histogram of the area surrounding the textbox rect
	cv::Mat getSurroundingLuminanceHistogram(int marginX = 3, int marginY = 3);

	cv::Rect getRect() { return textboxRect; }
	cv::Mat getSubmatrix() { return submatrix; }
};