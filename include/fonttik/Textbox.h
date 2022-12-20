//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "Frame.h"
#include <opencv2/core.hpp>


namespace tik {
	
class Textbox {

public:
	/* Operator method
	* Returns the percentage of overlap in each axis between two rectangles in relation to the smallest one
	* @return (x axis overlap, y axis overlap)
	* */
	static std::pair<float, float> OverlapAxisPercentage(const Textbox& a, const Textbox& b);
	bool operator==(const Textbox& b) const{
		return textboxRect == b.textboxRect && parentImage == b.parentImage;
	}

	Textbox(const std::vector<cv::Point >& points, int padding = 0);
	Textbox(cv::Rect rect);
	//Sets textbox's parent image and calculates its submatrix
	void setParentMedia(Frame* media);

	/// <summary>
	/// Changes this textbox into one that includes itself and another
	/// </summary>
	/// <param name="other">absorbed textbox</param>
	void mergeWith(Textbox& other) { textboxRect = textboxRect | other.getRect(); };

	//Returns and saves the luminance histogram of the specified textbox rect
	cv::Mat getLuminanceHistogram();

	//Returns the luminance histogram of the area surrounding the textbox rect
	cv::Mat getSurroundingLuminanceHistogram(int marginX = 3, int marginY = 3);

	cv::Rect getRect() const { return textboxRect; }
	cv::Mat getSubmatrix() const { return submatrix; }
	cv::Mat getLuminanceMap() const { return parentImage->getFrameLuminance()(textboxRect); };
	cv::Mat getTextMask();

protected:
	cv::Rect textboxRect;

	cv::Mat submatrix;
	cv::Mat luminanceHistogram;
	cv::Mat textMask; //Shows where text is located in the image (text as 1s and not text as 0s)

	Frame* parentImage = nullptr;

	/// <summary>
	/// Determines where text is located
	/// </summary>
	/// <returns></returns>
	cv::Mat calculateTextMask();
};

}