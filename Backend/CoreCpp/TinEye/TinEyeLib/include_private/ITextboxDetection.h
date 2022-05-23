#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include "Textbox.h"

namespace tin {
	class AppSettings;
	class TextDetectionParams;

	class ITextboxDetection {
	protected:
		// Calculates the angle of tilt of a textbox given two points (top or bottom side)
		static float HorizontalTiltAngle(const cv::Point& a, const cv::Point& b);
	
		//Initialize textbox detection with configuration parameters, must be called before any detection calls
		ITextboxDetection() {};
	public:
		virtual void init(const TextDetectionParams* params) = 0;
		//Releases memory used by textbox detection model
		virtual ~ITextboxDetection() {};

		virtual std::vector<Textbox> detectBoxes(const cv::Mat& img, const AppSettings* appSettigs, const TextDetectionParams* params) = 0;

		//Merges textboxes given a certain threshold for horizontal and vertical overlap
		static void mergeTextBoxes(std::vector<Textbox>& textBoxes, const TextDetectionParams* params);
	};

}