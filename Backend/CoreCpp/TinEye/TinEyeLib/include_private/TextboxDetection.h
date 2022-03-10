#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include "Textbox.h"

namespace tin {
	class AppSettings;
	class TextDetectionParams;

	class TextboxDetection {
	protected:
		static cv::dnn::TextDetectionModel_EAST* east;

		static void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);

		// Calcultes the angle of tilt of a textbox given two points (top or bottom side)
		static float HorizontalTiltAngle(const cv::Point& a, const cv::Point& b);
	public:
		//Initialize textbox detection with configuration parameters, must be called before any detection calls
		//Must be released later
		static void init(const TextDetectionParams* params);

		//Releases memory used by textbox detection model
		static void release();

		static std::vector<Textbox> detectBoxes(cv::Mat img, const AppSettings* appSettigs, const TextDetectionParams* params);

		static void mergeTextBoxes(std::vector<Textbox>& textBoxes, const TextDetectionParams* params);
	};

}