//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "SizeChecker.hpp"
#include "fonttik/Media.hpp"
#include "fonttik/TextBox.hpp"
#include "fonttik/Log.h"
#include "fonttik/Configuration.hpp"
#include "fonttik/ConfigurationParams.hpp"

#include <regex>

const std::regex ascenders("[ABCDEFGHIJKLMNOPQRSTUVWXYZbdfhijklt]"); //All characters with ascender
const std::regex descenders("[gjpqy]"); //Characters with descenders

namespace tik
{
	SizeChecker::SizeChecker(Configuration* config, ITextBoxRecognition* textboxRecognition) 
		: IChecker(config), textboxRecognition(textboxRecognition)
	{
	}

	FrameResults tik::SizeChecker::check(const int& frameIndex, std::vector<TextBox>& textBoxes)
	{
		FrameResults sizeResults(frameIndex);
		bool passes = true;

		//Run size check for each textbox in image
		for (TextBox& textBox : textBoxes)
		{
			bool individualPass = textBoxSizeCheck(textBox, sizeResults);

			passes = passes && individualPass;
		}

		sizeResults.overallPass = passes;
		return sizeResults;
	}

	bool SizeChecker::textBoxSizeCheck(TextBox& textBox, FrameResults& results)
	{
		bool sizeResult = true;
		cv::Rect textRect = textBox.getTextRect();
		cv::Rect boxRect = textBox.getTextBoxRect();

		ResultType type = ResultType::PASS;
		int measuredHeight = textRect.height;
		bool canFail = true; //only words with both ascenders and descenders present can fail

		
		//If not using text recognition, text height is chekced by accepting word as full-height
		std::string recognitionResult = textBox.getText();
		if (configuration->getTextSizeParams().useTextRecognition)
		{
			if(recognitionResult=="")
				recognizeText(recognitionResult, textBox);

			//Check for ascender or descender presence with regex
			bool hasAscender = std::regex_search(recognitionResult, ascenders);
			bool hasDescender = std::regex_search(recognitionResult, descenders);
			canFail = hasAscender && hasDescender;
		}

		//Check height
		int minimumHeight = configuration->getTextSizeParams().activeGuideline->height;

		//Check for minimum height based on guidelines
		if (measuredHeight < minimumHeight && canFail) 
		{
			sizeResult = false;
			type = ResultType::FAIL;
			LOG_CORE_TRACE("Word at ({0}, {1}) doesn't comply with minimum height {2}, detected height: {3}.{4}",
				boxRect.x, boxRect.y,
				minimumHeight, measuredHeight,
				((recognitionResult == "") ? "" : " Detected text: " + recognitionResult));
		}
		else if (measuredHeight < minimumHeight) 
		{
			type = ResultType::WARNING;
		}

		if (type == ResultType::FAIL && configuration->getAppSettings().failsAsWarnings)
		{
			type = ResultType::WARNING;
		}

		//The min and max values for x and y work as offsets inside the textbox, that's why original boxRect has to be accounted for.
		//-1 and +2 values to add margin accounting for the outline width
		results.results.push_back(ResultBox(type, boxRect.x + textRect.x - 1, boxRect.y + textRect.y - 1, textRect.width + 2, textRect.height + 2, measuredHeight, recognitionResult));
		return sizeResult;
	}

	void SizeChecker::recognizeText(std::string& recognitionResult, TextBox& textBox)
	{
		//Recognize word in region
		recognitionResult = textboxRecognition->recognizeBox(textBox);
		textBox.setText(recognitionResult);

		//Clean \r invalid characters
		size_t pos{};
		while (((pos = recognitionResult.find('\r')) != std::string::npos))
			recognitionResult.erase(pos, 1);
	}

}