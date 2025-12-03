//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/Frame.hpp"
#include "fonttik/Media.hpp"
#include "fonttik/Fonttik.hpp"
#include "Video.hpp"
#include "Image.hpp"
#include <opencv2/imgproc.hpp>

#include <fstream>

namespace tik {

Frame::Frame(cv::Mat img, cv::Mat mask) : image(img)
{
	applyMask(mask);
}

Frame::Frame(cv::Mat img, cv::Mat mask, const int frameIndx) : Frame(img, mask)
{
	frameIndex = frameIndx;
}

Frame::Frame(cv::Mat img, cv::Mat mask, const int frameIndx, int& msTimeSpamp) : Frame(img, mask, frameIndx)
{
	timeStamp = convertToTimeSpan(msTimeSpamp);
}

Frame::~Frame() 
{
		
}

void Frame::applyMask(cv::Mat mask)
{
	if(!mask.empty())
	{
		image = image & mask;
	}
}

void Frame::paintTextBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& image, int thickness)
{
	cv::rectangle(image, cv::Rect{ x1,y1,x2,y2 }, color, thickness);
}

void Frame::paintTextBoxResultValues(cv::Mat& image, const ResultBox& box, double value, int precision)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << value; //remove trailing zeros when converting to string
	std::string text = stream.str();

	//Calculate text size
	int baseline;
	cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 1, 3, &baseline);

	//Check to see if text fits to the right of the textbox, if not, put it to the left
	cv::Point originPoint = (box.x + box.width + textSize.width < image.cols) ?
		cv::Point(box.x + box.width, box.y + box.height) : cv::Point(box.x - textSize.width, box.y + box.height);

	//Put the text into the image
	cv::putText(image, text, originPoint, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(), 3);
	cv::putText(image, text, originPoint, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 1);
}

std::string Frame::convertToTimeSpan(const int& msTimeSpan)
{
	float seconds = fmodf((msTimeSpan / 1000.0), 60);
	int minutes = floor((msTimeSpan / (1000 * 60)) % 60);
	int hours = floor((msTimeSpan / (1000 * 60 * 60)) % 24);

	std::string h = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
	std::string m = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);
	std::string s = (seconds < 10) ? "0" + std::to_string(seconds) : std::to_string(seconds);

	return h + ":" + m + ":" + s;
}
}