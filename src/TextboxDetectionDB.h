// Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include "ITextboxDetection.h"

namespace tik {
class TextDetectionParams;

class TextboxDetectionDB : public ITextboxDetection {

public:
	TextboxDetectionDB(const TextDetectionParams& params) :ITextboxDetection(params) {};

	//Initialize textbox detection with configuration parameters, must be called before any detection calls
	virtual void init(const std::vector<double>& sRGB_LUT);

	//Releases memory used by textbox detection model
	virtual ~TextboxDetectionDB();

	virtual std::vector<TextBox> detectBoxes(const cv::Mat& img);
	virtual LinesAndWords detectLinesAndWords(const cv::Mat& img);

private:

	cv::dnn::TextDetectionModel_DB* db;
};

}
