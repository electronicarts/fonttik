//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include "fonttik/Textbox.h"

namespace tik {
	
class AppSettings;
class TextDetectionParams;

class ITextboxDetection {

public:
	virtual void init(const TextDetectionParams* params, const AppSettings* appSettingsCfg) = 0;
	//Releases memory used by textbox detection model
	virtual ~ITextboxDetection() {};

	virtual std::vector<Textbox> detectBoxes(const cv::Mat& img) = 0;

	//Merges textboxes given a certain threshold for horizontal and vertical overlap
	static void mergeTextBoxes(std::vector<Textbox>& textBoxes, const TextDetectionParams* params);

protected:
	// Calculates the angle of tilt of a textbox given two points (top or bottom side)
	static float HorizontalTiltAngle(const cv::Point& a, const cv::Point& b);

	//Initialize textbox detection with configuration parameters, must be called before any detection calls
	ITextboxDetection() {};

	//Caching of appSettings and textDetectionParams pointers
	const TextDetectionParams* detectionParams = nullptr;
	const AppSettings* appSettings = nullptr;
};

}