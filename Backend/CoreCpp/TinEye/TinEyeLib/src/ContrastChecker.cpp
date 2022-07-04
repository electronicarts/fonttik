//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "ContrastChecker.h"
#include "Instrumentor.h"
#include "TinEye.h"

namespace tin {

	FrameResults tin::ContrastChecker::check(Frame& frame, std::vector<Textbox>& boxes)
	{
		PROFILE_FUNCTION();

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
				PROFILE_SCOPE("saveHistograms");
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
		PROFILE_FUNCTION();
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
			//BOOST_LOG_TRIVIAL(info) << "Mask positive: " << cv::countNonZero(textMask) << " mask negative: " << textMask.rows * textMask.cols - cv::countNonZero(textMask) << std::endl;
		}
#endif // _DEBUG

		double ratio = TinEye::ContrastBetweenRegions(luminanceRegion, textMask, outlineMask);

		ResultType type = ResultType::PASS;
		bool boxPasses = ratio >= config->getGuideline()->getContrastRequirement();

		if (!boxPasses) {
			type = ResultType::FAIL;
			BOOST_LOG_TRIVIAL(info) << "Word: " << boxRect << " doesn't comply with minimum luminance contrast " << config->getGuideline()->getContrastRequirement()
				<< ", detected contrast ratio is " << ratio << " at: " << boxRect << std::endl;
		}
		else if (ratio < config->getGuideline()->getContrastRecommendation()) {
			type = ResultType::WARNING;
		}

		results.results.push_back(ResultBox(type, boxRect.x, boxRect.y, boxRect.width, boxRect.height, ratio));

		return boxPasses;
	}
}