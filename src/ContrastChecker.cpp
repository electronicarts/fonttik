//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "ContrastChecker.hpp"
#include "fonttik/Log.h"
#include "fonttik/Configuration.hpp"

namespace tik
{

FrameResults tik::ContrastChecker::check(const int& frameIndex, std::vector<TextBox>& textBoxes, std::vector<std::vector<TextBox>> colorblindBoxes)
{
	//add entry for this frame in result struct
	FrameResults contrastResults(frameIndex);

	//Run contrast check for each textbox in image
	for (int i = 0; i < textBoxes.size(); i++)
	{
		TextBox textBox = textBoxes[i];
		cv::Mat textMask = textBox.getTextMask();
		cv::Mat outlineMask;

		//Dilate and then substract textMask to get the outline of the text
		int dilationSize = configuration->getContrastRatioParams().textBackgroundRadius * 2 + 1;
		cv::dilate(textMask, outlineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize)));

		//To prevent antialising messing with measurements we expand the mask to be subtracted
		cv::Mat substraction;
		cv::dilate(textMask, substraction, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
		outlineMask -= substraction;

		std::pair<tik::ResultType, double> results = textboxContrastCheck(textBox, textMask, outlineMask);

		if (colorblindBoxes.empty()) {
			contrastResults.results.push_back(ResultBox(results.first, textBox.getTextBoxRect(), results.second));
		}
		else {
			std::vector<TextBox> colorblindTextBoxes = colorblindBoxes[i];
			std::vector<double> colorblindRatios;
			std::vector<ResultType> colorblindTypes;
			for (int j = 0; j < 4; j++)
			{
				TextBox& colorblindTextBox = colorblindTextBoxes[j];
				std::pair<tik::ResultType, double> colorblindResults = textboxContrastCheck(colorblindTextBox, textMask, outlineMask);
				colorblindTypes.push_back(colorblindResults.first);
				colorblindRatios.push_back(colorblindResults.second);
				contrastResults.overallColorblindPass[j] = contrastResults.overallColorblindPass[j] && (colorblindResults.first == ResultType::PASS);
				contrastResults.overallColorblindType[j] = ResultTypeMerge(contrastResults.overallColorblindType[j], colorblindResults.first);
			}
			contrastResults.results.push_back(ResultBox(results.first, textBox.getTextBoxRect(), results.second, colorblindRatios, colorblindTypes));
		}

		bool boxPasses = results.first == ResultType::PASS;
		contrastResults.overallPass = contrastResults.overallPass && boxPasses;
		contrastResults.overallType = ResultTypeMerge(contrastResults.overallType, results.first);
		contrastResults.results.back().text = textBox.getText();
	}
	
	return contrastResults;
}

std::pair<tik::ResultType, double> tik::ContrastChecker::textboxContrastCheck(TextBox& textBox, cv::Mat textMask, cv::Mat outlineMask)
{
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

	if (type == ResultType::FAIL && configuration->getAppSettings().failsAsWarnings)
	{
		type = ResultType::WARNING;
	}

	return { type, ratio };
}

double ContrastChecker::getContrastBetweenRegions(const cv::Mat& luminance, const cv::Mat& textMask, const cv::Mat& outlineMask)
{
	//Calculate the mean of the luminance for the light regions of the luminance
	double meanLight = cv::mean(luminance, textMask)[0];

	double meanDark = cv::mean(luminance, outlineMask)[0];

	return (std::max(meanLight, meanDark) + 0.05) / (std::min(meanLight, meanDark) + 0.05);
}

}