#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include "Textbox.h"

class TextboxDetection {
protected:
	static void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);
public:
	static std::vector< Textbox > detectBoxes(cv::Mat img,bool saveBoxesImage =false);
};