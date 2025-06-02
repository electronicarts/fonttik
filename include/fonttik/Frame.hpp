//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <opencv2/opencv.hpp>
#include "fonttik/Results.h"
#include <filesystem>
#include <string>

namespace tik 
{

class Media;

//Holds the necessary information to 
class Frame {

public:
	Frame(cv::Mat img, cv::Mat mask);

	Frame(cv::Mat img, cv::Mat mask, const int frameIndx);

	Frame(cv::Mat img, cv::Mat mask, const int frameIndx, int& msTimeSpamp);

	void applyMask(cv::Mat mask);

	cv::Mat getFrameMat() { return image; }
		
	inline int getFrameIndex() { return frameIndex; }

	inline std::string getTimeStamp() { return timeStamp; }

	//Paints the outline of the TextBox in a specified image
	static void paintTextBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& image, int thickness = 1);

	//Paints output value from a resultbox next to itself in an image
	static void paintTextBoxResultValues(cv::Mat& image, const ResultBox& box, int precision);

	virtual ~Frame();

private:
		
	std::string convertToTimeSpan(const int& msTimeSpan);
	//apply mask info

	cv::Mat image;
	int frameIndex;
	std::string timeStamp;
};

}