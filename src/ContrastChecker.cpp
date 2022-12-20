//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "ContrastChecker.h"
#include "fonttik/Fonttik.h"
#include "fonttik/Log.h"


namespace tik {

	FrameResults tik::ContrastChecker::check(Frame& frame, std::vector<Textbox>& boxes)
	{
		cv::Mat imageMatrix = frame.getImageMatrix();
		cv::Mat luminanceMap = frame.getFrameLuminance();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();

#ifdef _DEBUG
		int counter = 0;
#endif // _DEBUG

		bool imagePasses = true;

		//add entry for this frame in result struct
		FrameResults contrastResults(frame.getFrameNumber());

		//Run contrast check for each textbox in image
		for (Textbox box : boxes) {

			box.setParentMedia(&frame);

			bool individualPass = textboxContrastCheck(frame, box, contrastResults);


#ifdef _DEBUG
			if (appSettings->saveHistograms()) {
				cv::Rect boxRect = box.getRect();
				fs::path savePath = frame.getMedia()->getOutputPath() / ("img" + std::to_string(counter) + "histogram.png");
				Frame::saveLuminanceHistogram(box.getLuminanceHistogram(),
					savePath.string());

				Frame::saveHistogramCSV(frame.calculateLuminanceHistogram(boxRect), frame.getPath().replace_filename("histogram" + std::to_string(counter) + ".csv").string());
			}
			counter++;

#endif
			imagePasses = imagePasses && individualPass;

		}
		contrastResults.overallPass = imagePasses;

		return contrastResults;
	}

	bool ContrastChecker::textboxContrastCheck(Frame& image, Textbox& textbox, FrameResults& results) {
		cv::Rect boxRect = textbox.getRect();

		//Contrast checking with thresholds
		cv::Mat textMask, outlineMask;
		cv::Mat luminanceRegion = textbox.getLuminanceMap();
		textMask = textbox.getTextMask();

		//Dilate and then substract textMask to get the outline of the text
		int dilationSize = config->getGuideline()->getTextBackgroundRadius() * 2 + 1;
		cv::dilate(textMask, outlineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize)));
		
		//To prevent antialising messing with measurements we expand the mask to be subtracted
		cv::Mat substraction;
		cv::dilate(textMask, substraction, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
		outlineMask -= substraction;

#ifdef _DEBUG
		if (config->getAppSettings()->saveLuminanceMasks()) {
			//frame.saveOutputData(luminanceRegion, "lum.png");
			image.saveOutputData(textMask, image.getMedia()->getOutputPath() / ("mask" + std::to_string(textbox.getRect().x) + ".png"));
			//LOG_CORE_INFO("Mask positive: {0} mask negative: {1}", cv::countNonZero(textMask), textMask.rows * textMask.cols - cv::countNonZero(textMask));
		}
#endif // _DEBUG

		double ratio = Fonttik::ContrastBetweenRegions(luminanceRegion, textMask, outlineMask);

		ResultType type = ResultType::PASS;
		bool boxPasses = ratio >= config->getGuideline()->getContrastRequirement();

		if (!boxPasses) {
			type = ResultType::FAIL;
			LOG_CORE_INFO("Word: {0}  doesn't comply with minimum luminance contrast {1}, detected contrast ratio is {2} at: {0}", boxRect, config->getGuideline()->getContrastRequirement(), ratio);
		}
		else if (ratio < config->getGuideline()->getContrastRecommendation()) {
			type = ResultType::WARNING;
		}

		results.results.push_back(ResultBox(type, boxRect.x, boxRect.y, boxRect.width, boxRect.height, ratio));

		return boxPasses;
	}
}