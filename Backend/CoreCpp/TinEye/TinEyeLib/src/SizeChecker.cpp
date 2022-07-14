//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "SizeChecker.h"
#include "Instrumentor.h"
#include "Media.h"
#include <regex>

const std::regex ascenders("[bdfhkltA-Z0-9]"); //All characters with ascender
const std::regex descenders("[gjpqy]"); //Characters with descenders

namespace tin {
	FrameResults tin::SizeChecker::check(Frame& image, std::vector<Textbox>& boxes)
	{
		PROFILE_FUNCTION();
		cv::Mat openCVMat = image.getImageMatrix();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();
		FrameResults sizeResults(image.getFrameNumber());

		//Check if user has set manual source resolution or if using DPI guidelines
		//Set guideline values accordingly
		//If DPI is off and no manual resolution is set, resolution will be extracted from image itself
		int activeSize = appSettings->getSpecifiedSize();
		guideline->setDPI(appSettings->usingDPI());
		guideline->setActiveGuideline((activeSize != 0) ? activeSize : openCVMat.rows);

		bool passes = true;

#ifdef _DEBUG
		int counter = 0;
#endif


		//Run size check for each textbox in image
		for (Textbox box : boxes) {
			box.setParentMedia(&image);

			bool individualPass = textboxSizeCheck(image, box, sizeResults);

			passes = passes && individualPass;

#ifdef _DEBUG
			if (appSettings->saveSeparateTextboxes()) {
				image.saveOutputData(box.getSubmatrix(), image.getMedia()->getOutputPath() / ("textbox_" + std::to_string(counter) + ".png"));
			}
			counter++;

#endif
		}

		sizeResults.overallPass = passes;
		return sizeResults;
	}

	bool tin::SizeChecker::textboxSizeCheck(Frame& image, Textbox& textbox, FrameResults& results)
	{
		PROFILE_FUNCTION();
		bool pass = true;
		cv::Rect boxRect = textbox.getRect();
		cv::Mat textMask = textbox.getTextMask();

		ResultType type = ResultType::PASS;

		/*Calculate heightand width more accurately:
		Instead of using the size of the rect, we use the outermost positions of the text mask
		with highest and lowest x and y coordinates, which represent the edges of the text.
		*/
		std::vector<cv::Point> nonZero;
		cv::findNonZero(textMask, nonZero);
		int minY = std::numeric_limits<int>::max(),
			maxY = std::numeric_limits<int>::min(),
			minX = std::numeric_limits<int>::max(),
			maxX = std::numeric_limits<int>::min();
		for (const auto& point : nonZero) {
			minY = std::min(point.y, minY);
			minX = std::min(point.x, minX);
			maxY = std::max(point.y, maxY);
			maxX = std::max(point.x, maxX);
		}

		int height = maxY - minY, measuredHeight = height;

		//If new calculated height is smaller than textbox add a trace
		if (height < boxRect.height) {
			BOOST_LOG_TRIVIAL(trace) << "Removed vertical overhead " << boxRect.height - height << " px at " << boxRect.x << ", " << boxRect.y << std::endl;
		}

		//If not using text recognition test height is chekced by accepting word as full-height
		if (config->getAppSettings()->textRecognitionActive()) {
			//bool hasAscender = false, hasDescender = false;

			//Recognize word in region
			std::string recognitionResult;
			recognitionResult = textboxRecognition->recognizeBox(textbox);


			//Average width is no longer checked for due to legal saying it's not necessary.
			/*int width = maxX - minX;
			//Avoids division by zero
			int averageWidth = (recognitionResult.size() > 0) ? width / recognitionResult.size() : -1;


			Check average width
			if (averageWidth == -1) {
				BOOST_LOG_TRIVIAL(warning) << "Text inside " << boxRect << " couldn't be recognized, it is suggested to increase the text recognition minimum confidence" << std::endl;
				type = ResultType::UNRECOGNIZED;
			}
			else if (averageWidth < config->getGuideline()->getWidthRequirement()) {
				pass = false;
				type = ResultType::FAIL;
				BOOST_LOG_TRIVIAL(info) << "Average character width for word: " << recognitionResult << " doesn't comply with minimum width, detected width: " << averageWidth <<
					" at (" << boxRect.x << ", " << boxRect.y << ")" << std::endl;
			}
			else if (averageWidth < config->getGuideline()->getWidthRecommendation()) {
				type = ResultType::WARNING;
			}*/

			//Check for ascender or descender presence with regex
			bool hasAscender = std::regex_search(recognitionResult, ascenders);
			bool hasDescender = std::regex_search(recognitionResult, descenders);

			//TODO use configuration values, starting off with 1:3:1 ratio by default
			float ascRatio = 1, descRatio = 1, xRatio = 3;
			float ratioTotal = ascRatio + descRatio + xRatio;
			float accumulatedRatio = ((hasAscender) ? ascRatio : 0) + ((hasDescender) ? descRatio : 0) + xRatio;

			//size in the output will be full word size based on actual measurements
			measuredHeight = ratioTotal * measuredHeight / accumulatedRatio;
		}

		//Check height
		int minimumHeight = config->getGuideline()->getHeightRequirement();

		//Check for minimum height based on guidelines
		if (measuredHeight < minimumHeight) {
			pass = false;
			type = ResultType::FAIL;
			BOOST_LOG_TRIVIAL(info) << "Word at (" << boxRect.x << ", " << boxRect.y << ") doesn't comply with minimum height "
				<< minimumHeight << ", detected height: " << height << std::endl;
		}
		else if (measuredHeight < config->getGuideline()->getHeightRecommendation() && pass) {
			//Check for recommended guidelines
			type = ResultType::WARNING;
		}



		//The min and max values for x and y work as offsets inside the textbox, that's why original boxRect has to be accounted for.
		//-1 and +2 values to add margin accounting for the outline width
		results.results.push_back(ResultBox(type, boxRect.x + minX - 1, boxRect.y + minY - 1, maxX - minX + 2, height + 2, measuredHeight));

		return pass;
	}
}