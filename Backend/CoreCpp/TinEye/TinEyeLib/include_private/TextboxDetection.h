#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include "Textbox.h"

class AppSettings;
class TextDetectionParams;

class TextboxDetection {
protected:
	static void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);
public:
	static std::vector<Textbox> detectBoxes(cv::Mat img, const AppSettings* appSettigs, const TextDetectionParams* params);

	static void mergeTextBoxes(std::vector<Textbox>& textBoxes, const TextDetectionParams* params);
};