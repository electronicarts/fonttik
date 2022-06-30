//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "Configuration.h"
#include "AppSettings.h"
#include "Guideline.h"
#include "TextDetectionParams.h"

namespace tin {
	Configuration::Configuration() {
		appSettings = AppSettings();
		guideline = Guideline();
		textDetectionParams = TextDetectionParams();
		textRecognitionParams = TextRecognitionParams();
	}

	Configuration::Configuration(fs::path configPath) : Configuration() {
		std::ifstream configFile(configPath);

		if (configFile) {
			json config;
			configFile >> config;
			//Guidelines
			try {
				guideline.init(config["guideline"]);
			}
			catch (...) {
				logLoadingError("Guidelines ");
				guideline = Guideline();
			}
			//AppSettings
			try {
				appSettings.init(config["appSettings"]);
			}
			catch (...) {
				logLoadingError("Application settings");
				appSettings = AppSettings();
			}
			//Text Detection
			try {
				textDetectionParams.init(config["textDetection"]);
			}
			catch (...) {
				logLoadingError("Text detection parameters");
				textDetectionParams = TextDetectionParams();
			}
			//text recognition
			try {
				textRecognitionParams.init(config["textRecognition"]);
			}
			catch (...) {
				logLoadingError("Text recognition parameters");
				textRecognitionParams = TextRecognitionParams();
			}
			//RGB lookup tables
			try {

				for (auto& elem : config["sRGBLinearizationValues"]) {
					rgbLookUp.emplace_back(elem);
				}
			}
			catch (...) {
			}
		}
		else {
			BOOST_LOG_TRIVIAL(error) << "ATTENTION! Configuration file was not found. ALL configurable values will use DEFAULT configuration." << std::endl;
		}
	}

	void Configuration::logLoadingError(std::string name) {
		BOOST_LOG_TRIVIAL(error) << "ATTENTION! There was a problem loading: " << name
			<< ", one or more of its value are possibly malformed or missing.\n" << std::endl;
		BOOST_LOG_TRIVIAL(error) << name << " configuration values will be reverted to DEFAULT configuration during this execution." << std::endl;
	}
}
