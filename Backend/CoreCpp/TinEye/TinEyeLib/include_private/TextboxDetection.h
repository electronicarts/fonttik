#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include "Textbox.h"

namespace tin {
	class AppSettings;
	class TextDetectionParams;

	class TextboxDetection {
	protected:
		cv::dnn::TextDetectionModel_EAST* east;

		static void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);

		// Calculates the angle of tilt of a textbox given two points (top or bottom side)
		static float HorizontalTiltAngle(const cv::Point& a, const cv::Point& b);
	public:
		//Initialize textbox detection with configuration parameters, must be called before any detection calls
		TextboxDetection(const TextDetectionParams* params);

		//Releases memory used by textbox detection model
		~TextboxDetection();

		std::vector<Textbox> detectBoxes(cv::Mat img, const AppSettings* appSettigs, const TextDetectionParams* params);

		//Merges textboxes given a certain threshold for horizontal and vertical overlap
		static void mergeTextBoxes(std::vector<Textbox>& textBoxes, const TextDetectionParams* params);
	};

}