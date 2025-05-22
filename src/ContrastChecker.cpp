//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "ContrastChecker.hpp"
#include "fonttik/Log.h"
#include "fonttik/Configuration.hpp"

namespace tik
{

FrameResults tik::ContrastChecker::check(const int& frameIndex, std::vector<TextBox>& textBoxes)
{
	//add entry for this frame in result struct
	FrameResults contrastResults(frameIndex);
	
	bool passes = true;

	//Run contrast check for each textbox in image
	for (TextBox& textBox : textBoxes) {

		bool individualPass = textboxContrastCheck(textBox, contrastResults);

		passes = passes && individualPass;
		contrastResults.results.back().text = textBox.getText();
	}

	contrastResults.overallPass = passes;

	return contrastResults;
}

bool tik::ContrastChecker::textboxContrastCheck(TextBox& textBox, FrameResults& results)
{
	cv::Mat textMask = textBox.getTextMask();
	cv::Mat outlineMask;

	//Dilate and then substract textMask to get the outline of the text
	int dilationSize = configuration->getContrastRatioParams().textBackgroundRadius * 2 + 1;
	cv::dilate(textMask, outlineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize)));

	//To prevent antialising messing with measurements we expand the mask to be subtracted
	cv::Mat substraction;
	cv::dilate(textMask, substraction, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	outlineMask -= substraction;

	double ratio = getContrastBetweenRegions(textBox.getTextMatLuminance(), textMask, outlineMask);
	ratio = double((int)(ratio * 10)) / 10; //ceil floating point numbers to one decimal

	ResultType type = ResultType::PASS;
	bool boxPasses = ratio >= configuration->getContrastRatioParams().contrastRatio;

	if (!boxPasses) 
	{
		type = ResultType::FAIL;
		LOG_CORE_TRACE("Word: {0}  doesn't comply with minimum luminance contrast {1}, detected contrast ratio is {2} at: {0}", 
			textBox.getTextBoxRect(), configuration->getContrastRatioParams().contrastRatio, ratio);
	}
	else if (ratio < configuration->getContrastRatioParams().contrastRatio)
	{
		type = ResultType::WARNING;
	}

	if (type == ResultType::FAIL && configuration->getAppSettings().failsAsWarnings)
	{
		type = ResultType::WARNING;
	}
	results.results.push_back(ResultBox(type, textBox.getTextBoxRect().x, textBox.getTextBoxRect().y, 
		textBox.getTextBoxRect().width, textBox.getTextBoxRect().height, ratio));

	return boxPasses;
}

double ContrastChecker::getContrastBetweenRegions(const cv::Mat& luminance, const cv::Mat& textMask, const cv::Mat& outlineMask)
{
	//Calculate the mean of the luminance for the light regions of the luminance
	double meanLight = cv::mean(luminance, textMask)[0];

	double meanDark = cv::mean(luminance, outlineMask)[0];

	return (std::max(meanLight, meanDark) + 0.05) / (std::min(meanLight, meanDark) + 0.05);
}

}