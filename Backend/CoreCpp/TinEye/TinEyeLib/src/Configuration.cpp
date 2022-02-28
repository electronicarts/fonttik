#include "Configuration.h"
#include "AppSettings.h"
#include "Guideline.h"

Configuration::Configuration() {
	setDefaultAppSettings();
	setDefaultGuideline();
}

Configuration::Configuration(fs::path configPath) {
	std::ifstream configFile(configPath);
	if (configFile) {
		json config;
		configFile >> config;
		try {
			json guidelineJson = config["guideline"];
			std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines;
			for (auto it = guidelineJson["resolutions"].begin(); it != guidelineJson["resolutions"].end(); ++it)
			{
				ResolutionGuidelines a(it.value()["width"], it.value()["height"]);
				resolutionGuidelines[atoi(it.key().c_str())] = a;
			}
			guideline = new Guideline(guidelineJson["contrast"], resolutionGuidelines);

		}
		catch (...) {
			BOOST_LOG_TRIVIAL(error) << "Malformed configuration guidelines" << std::endl;
			setDefaultGuideline();
		}
		try {
			json settings = config["appSettings"];
			appSettings = new AppSettings(settings["saveLuminanceMap"], settings["saveTextboxOutline"],
				settings["saveSeparateTexboxes"], settings["saveHistograms"],settings["saveRawTextboxOutline"],
				settings["resultsPath"], settings["debugInfoPath"]);
		}
		catch (...) {
			BOOST_LOG_TRIVIAL(error) << "Malformed configuration appSettings" << std::endl;
			setDefaultAppSettings();
		}
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "Configuration file not found" << std::endl;
		setDefaultGuideline();
		setDefaultAppSettings();
	}
}

void Configuration::setDefaultGuideline() {
	//If file is not found, error and set default values
	BOOST_LOG_TRIVIAL(error) << "Configuration file not found, falling back to default configuration\n";
	BOOST_LOG_TRIVIAL(error) << "Contrast ratio: 4.5, language: eng" << std::endl;

	if (guideline != nullptr) {
		delete guideline;
	}
	guideline = new Guideline(4.5, { {1080,{4,28}} });
}

void Configuration::setDefaultAppSettings() {
	if (appSettings != nullptr) {
		delete appSettings;
	}
	appSettings = new AppSettings(true, true, false, false,false, "./", "./debugInfo");
}

