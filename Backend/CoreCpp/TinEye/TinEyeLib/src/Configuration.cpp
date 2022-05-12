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
			guideline.init(config["guideline"]);
			//AppSettigns
			try {
				json settings = config["appSettings"];

				appSettings.init(config["appSettings"]);
			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration appSettings" << std::endl;
			}
			//Text Detection
			try {
				textDetectionParams.init(config["textDetection"]);
			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration text detection params" << std::endl;
			}
			//text recognition
			try {
				textRecognitionParams.init(config["textRecognition"]);
			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration: Text recognition params" << std::endl;
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
			BOOST_LOG_TRIVIAL(error) << "Configuration file not found" << std::endl;
		}
	}
}
