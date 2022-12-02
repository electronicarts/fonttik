//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include <nlohmann/json.hpp>
#include "TextboxDetectionDB.h"
#include "TextboxDetectionEAST.h"
#include "AppSettings.h"

using json = nlohmann::json;

namespace tik {
	
class TextboxDetectionFactory {
public:
	static ITextboxDetection* CreateTextboxDetection(const AppSettings* appSettings, const TextDetectionParams* params){
		ITextboxDetection* textboxDetection = nullptr;
		AppSettings::DetectionBackend detectionBackend = appSettings->getDetectionBackend();
		switch (detectionBackend)
		{
			case AppSettings::DetectionBackend::EAST:
				textboxDetection = new TextboxDetectionEAST();
				break;
			case AppSettings::DetectionBackend::DB:
				textboxDetection = new TextboxDetectionDB();
				break;
			default:
				break;
		}
			
		if (textboxDetection != nullptr) {
			textboxDetection->init(params,appSettings);
		}

		return textboxDetection;
	}
};

}