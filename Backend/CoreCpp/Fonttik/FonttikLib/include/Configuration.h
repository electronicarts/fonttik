//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>
#include "../include_private/AppSettings.h"
#include "../include_private/TextDetectionParams.h"
#include "../include_private/Guideline.h"
#include "../include_private/TextRecognitionParams.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace tik {

class Configuration {

public:
	Configuration();
	Configuration(fs::path configPath);

	AppSettings* getAppSettings() { return &appSettings; }
	Guideline* getGuideline() { return &guideline; }
	TextDetectionParams* getTextDetectionParams() { return &textDetectionParams; }
	TextRecognitionParams* getTextRecognitionParams() { return &textRecognitionParams; }
	std::vector<double>* getRGBLookupTable() { return &rgbLookUp; }

private:
	AppSettings appSettings; //Values for user preferences, such as what data to save
	Guideline guideline; //Values for text and contrast measurements
	TextDetectionParams textDetectionParams;//EAST detection and post processing
	TextRecognitionParams textRecognitionParams;//OpenCV's OCR recognition
	std::vector<double> rgbLookUp;//look up table to optimize color space transformation

	void logLoadingError(std::string name);
};

}