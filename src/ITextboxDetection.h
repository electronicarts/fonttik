//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include "fonttik/TextBox.hpp"

namespace tik {
	
struct LinesAndWords {
	std::vector<TextBox> lines;
	std::vector<TextBox> words;
};

class TextDetectionParams;

class ITextboxDetection {

public:
	virtual void init(const std::vector<double>& sRGB_LUT) = 0;
	//Releases memory used by textbox detection model
	virtual ~ITextboxDetection() {};

	virtual std::vector<TextBox> detectBoxes(const cv::Mat& img) = 0;
	virtual LinesAndWords detectLinesAndWords(const cv::Mat& img) = 0;

	//Merges textboxes given a certain threshold for horizontal and vertical overlap
	void mergeTextBoxes(std::vector<TextBox>& textBoxe, cv::Mat img);

protected:
	// Calculates the angle of tilt of a textbox given two points (top or bottom side)
	static float HorizontalTiltAngle(const cv::Point& a, const cv::Point& b);

	//Initialize textbox detection with configuration parameters, must be called before any detection calls
	ITextboxDetection(const TextDetectionParams& params) : detectionParams(&params) {};

	//Caching of appSettings and textDetectionParams pointers
	const TextDetectionParams* detectionParams;
	std::vector<double> sRGB_LUT;
};

}