#include "ContrastChecker.h"
#include "Instrumentor.h"
#include "TinEye.h"

namespace tin {

	bool tin::ContrastChecker::check(Media& image, std::vector<Textbox>& boxes)
	{
		PROFILE_FUNCTION();

		cv::Mat openCVMat = image.getImageMatrix();
		cv::Mat luminanceMap = image.getFrameLuminance();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();

#ifdef _DEBUG
		int counter = 0;
#endif // _DEBUG


		if (openCVMat.empty())
		{
			return false;
		}

		bool imagePasses = true;

		//add entry for this image in result struct
		FrameResults* results = image.getResultsPointer()->addContrastResults(image.getFrameCount());


		for (Textbox box : boxes) {

			box.setParentMedia(&image);

			bool individualPass = textboxContrastCheck(image, box,*results);


#ifdef _DEBUG
			if (appSettings->saveHistograms()) {
				PROFILE_SCOPE("saveHistograms");
				cv::Rect boxRect = box.getRect();
				fs::path savePath = image.getPath().replace_filename("img" + std::to_string(counter) + "histogram.png").string();
				Media::saveLuminanceHistogram(box.getLuminanceHistogram(),
					savePath.string());

				Media::saveHistogramCSV(image.calculateLuminanceHistogram(boxRect), image.getPath().replace_filename("histogram" + std::to_string(counter) + ".csv").string());
			}
			counter++;

#endif
			imagePasses = imagePasses && individualPass;

		}

		Results* overallResults = image.getResultsPointer();
		overallResults->setContrastPass(imagePasses && overallResults->contrastPass());
		return imagePasses;
	}

	bool ContrastChecker::textboxContrastCheck(Media& image, Textbox& textbox,FrameResults& results) {
		PROFILE_FUNCTION();
		cv::Rect boxRect = textbox.getRect();

		//Contrast checking with thresholds
		cv::Mat maskA, maskB;
		cv::Mat luminanceRegion = textbox.getLuminanceMap();
		maskA = textbox.getTextMask();

		//Dilate and then substract maskA to get the outline of the mask
		int dilationSize = config->getGuideline()->getTextBackgroundRadius() * 2 + 1;
		cv::dilate(maskA, maskB, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize)));
		//To prevent antialising messing with measurements we expand the mask to be subtracted
		cv::Mat substraction;
		cv::dilate(maskA, substraction, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
		maskB -= substraction;

#ifdef _DEBUG
		if (config->getAppSettings()->saveLuminanceMasks()) {
			image.saveOutputData(luminanceRegion, "lum.png");
			image.saveOutputData(maskA, "mask.png");
			image.saveOutputData(maskB, "outlineMask.png");
		}
#endif // _DEBUG

		double ratio = TinEye::ContrastBetweenRegions(luminanceRegion, maskA, maskB);

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