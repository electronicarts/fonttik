//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "IChecker.h"


namespace tik 
{

class ContrastChecker : public IChecker 
{
public:
	ContrastChecker(Configuration* config) : IChecker(config) {}

	virtual ~ContrastChecker() {}

	virtual FrameResults check(const int& frameIndex, std::vector<TextBox>& textBoxes);

protected:
	bool textboxContrastCheck(TextBox& textbox, FrameResults& results);

	//Operator method
	//Calculates the contrast ratio of two given regions of a luminance matrix
public:
	static double getContrastBetweenRegions(const cv::Mat& luminance, const cv::Mat& textMask, const cv::Mat& outlineMask);
};

}
