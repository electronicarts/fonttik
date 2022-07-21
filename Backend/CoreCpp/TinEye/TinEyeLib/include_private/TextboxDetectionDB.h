// Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include "ITextboxDetection.h"

namespace tin {
	class AppSettings;
	class TextDetectionParams;

	class TextboxDetectionDB : public ITextboxDetection {

		cv::dnn::TextDetectionModel_DB* db = nullptr;

	public:
		TextboxDetectionDB() :ITextboxDetection() {};

		//Initialize textbox detection with configuration parameters, must be called before any detection calls
		virtual void init(const TextDetectionParams* params, const AppSettings* appSettingsCfg);

		//Releases memory used by textbox detection model
		virtual ~TextboxDetectionDB();

		virtual std::vector<Textbox> detectBoxes(const cv::Mat& img);
	};
}
