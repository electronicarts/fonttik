#include "SizeChecker.h"
#include "Instrumentor.h"

namespace tin {
	bool tin::SizeChecker::check(Media& image, std::vector<Textbox>& boxes)
	{
		PROFILE_FUNCTION();
		cv::Mat openCVMat = image.getImageMatrix();

		AppSettings* appSettings = config->getAppSettings();
		Guideline* guideline = config->getGuideline();

		if (openCVMat.empty())
		{
			return false;
		}

		guideline->setActiveResolution(openCVMat.rows);

		bool passes = true;

#ifdef _DEBUG
		int counter = 0;
#endif

		//add entry for this image in result struct
		Results* testResults = image.getResultsPointer();
		testResults->sizeResults.push_back({ image.getFrameCount(), std::vector<ResultBox>() });

		for (Textbox box : boxes) {
			//Set word detection to word bounding box
			box.setParentMedia(&image);

			bool individualPass = textboxSizeCheck(image, box);

			passes = passes && individualPass;

#ifdef _DEBUG
			if (appSettings->saveSeparateTextboxes()) {
				image.saveOutputData(box.getSubmatrix(), "textbox_" + std::to_string(counter) + ".png");
			}
			counter++;

#endif
		}

		return passes;
	}

	bool tin::SizeChecker::textboxSizeCheck(Media& image, Textbox& textbox)
	{
		PROFILE_FUNCTION();
		bool pass = true;
		cv::Rect boxRect = textbox.getRect();

		ResultType type = ResultType::PASS;

		//Calculate height and width precisely
		cv::Mat textMask = textbox.getTextMask();
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

		if (config->getAppSettings()->textRecognitionActive()) {
			//Recognize word in region
			std::string recognitionResult;
			recognitionResult = textboxRecognition->recognizeBox(textbox);

			int width = maxX - minX;
			//Avoids division by zero
			int averageWidth = (recognitionResult.size() > 0) ? width / recognitionResult.size() : -1;

			//Check average width
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
			}
		}

		//Check height
		int minimumHeight = config->getGuideline()->getHeightRequirement();
		int height = maxY - minY;
		if (height < boxRect.height) {
			BOOST_LOG_TRIVIAL(trace) << "Removed vertical overhead " << boxRect.height - height << " px at " << boxRect.x << ", " << boxRect.y << std::endl;
		}
		if (height < minimumHeight) {
			pass = false;
			type = ResultType::FAIL;
			BOOST_LOG_TRIVIAL(info) << "Word at (" << boxRect.x << ", " << boxRect.y << ") doesn't comply with minimum height "
				<< minimumHeight << ", detected height: " << height << std::endl;
		}
		else if (height < config->getGuideline()->getHeightRecommendation() && pass) {
			//Check for recommended guidelines
			type = ResultType::WARNING;
		}

		Results* testResults = image.getResultsPointer();
		testResults->sizeResults.back().second.push_back(ResultBox(type, boxRect.x, boxRect.y, boxRect.width, boxRect.height, height));
		testResults->overallSizePass = testResults->overallSizePass && pass;

		return pass;
	}
}