//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include <nlohmann/json.hpp>
#include "fonttik/Configuration.hpp"
#include "TextboxDetectionDB.h"
#include "TextboxDetectionEAST.h"
#include "ITextboxRecognition.h"
#include "TextBoxRecognitionOpenCV.hpp"

using json = nlohmann::json;

namespace tik {
	
class OCRFactory {
public:
	static ITextboxDetection* CreateTextboxDetection(const Configuration::DetectionBackend& detectionBackend, const TextDetectionParams& params, const std::vector<double>& sRGB_LUT)
	{
		ITextboxDetection* textboxDetection = nullptr;
		switch (detectionBackend)
		{
			case Configuration::DetectionBackend::DB_EAST:
				textboxDetection = new TextboxDetectionEAST(params);
				break;
			case Configuration::DetectionBackend::DB_DiffBinarization:
				textboxDetection = new TextboxDetectionDB(params);
				break;
			default:
				break;
		}
			
		if (textboxDetection != nullptr) {
			textboxDetection->init(sRGB_LUT);
		}

		return textboxDetection;
	}
	static ITextBoxRecognition* CreateTextboxRecognition(const TextRecognitionParams& params) {
		ITextBoxRecognition* rec = new TextBoxRecognitionOpenCV();
		rec->init(params);
		return rec;
	}	
};

}