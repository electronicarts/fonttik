//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "ITextboxDetection.h"

namespace tik {
	
class TextDetectionParams;

class TextboxDetectionEAST : public ITextboxDetection {

public:
	TextboxDetectionEAST(const TextDetectionParams& params) :ITextboxDetection(params) {};

	//Initialize textbox detection with configuration parameters, must be called before any detection calls
	virtual void init(const std::vector<double>& sRGB_LUT);

	//Releases memory used by textbox detection model
	virtual ~TextboxDetectionEAST();

	virtual std::vector<TextBox> detectBoxes(const cv::Mat& img);
	virtual LinesAndWords detectLinesAndWords(const cv::Mat& img);

protected:
	cv::dnn::TextDetectionModel_EAST* east;

	static void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);
};

}