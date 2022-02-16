#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include <map>
#include <filesystem>
#include <iostream>
#include <boost/log/trivial.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

struct ResolutionGuidelines {
	size_t width;
	size_t height;
	ResolutionGuidelines() :width(0), height(0) {}
	ResolutionGuidelines(size_t w, size_t h)
		:width(w), height(h) {}
};

class Configuration {
private:
	float contrastRatio;
	std::string language;
	std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines;
	ResolutionGuidelines* activeResolution = nullptr;
	fs::path tessdataPath = "./tessdata";
	fs::path trainingDataPath = "./tessdata/eng.traineddata";
	bool validLanguage = false;

	void setDefaultConfig() {
		//If file is not found, error and set default values
		std::cerr << "Configuration file not found, falling back to default configuration\n";
		std::cerr << "Contrast ratio: 4.5, language: eng" << std::endl;

		contrastRatio = 4.5f;
		setActiveLanguage("eng");
	}
public:
	Configuration() {};
	Configuration(fs::path configPath) {
		json config;
		std::ifstream configFile(configPath);
		if (configFile) {
			try {
				configFile >> config;
				contrastRatio = config["contrast"];
				tessdataPath = fs::path(std::string(config["tessdataPath"]));
				setActiveLanguage(config["language"]);

				for (auto it = config["resolutions"].begin(); it != config["resolutions"].end(); ++it)
				{
					ResolutionGuidelines a(it.value()["width"], it.value()["height"]);
					resolutionGuidelines[atoi(it.key().c_str())] = a;
				}
			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration file" << std::endl;
				setDefaultConfig();
			}
		}
		else {
			BOOST_LOG_TRIVIAL(error) << "Configuration file not found" << std::endl;
			setDefaultConfig();
		}

	}

	void setActiveResolution(int resolution) {
		auto foundRes = resolutionGuidelines.find(resolution);
		//Check if specified resolution was added during file load
		if (foundRes == resolutionGuidelines.end()) {
			//If not found, error and create default for 1080
			std::cerr << "Specified resolution not found, using 1080p, 28x4px as baseline" << std::endl;
			resolutionGuidelines[1080] = ResolutionGuidelines(4, 28);
			activeResolution = &resolutionGuidelines.find(1080)->second;
		}
		else {
			activeResolution = &foundRes->second;
		}
	};
	void setActiveLanguage(std::string lang) {
		fs::path path = tessdataPath / (lang + ".traineddata");
		if (fs::exists(path)) {
			BOOST_LOG_TRIVIAL(info) << path.native().c_str() << std::endl;
			language = lang;
			trainingDataPath = path;
			validLanguage = true;
		}
		else if (lang != "eng") {
			BOOST_LOG_TRIVIAL(error) << "No training data found for: " << lang << " ,defatulting to english" << std::endl;
			setActiveLanguage("eng");
		}
		else {
			BOOST_LOG_TRIVIAL(error) << "Active language is english, but no training data was found" << std::endl;
		}
	}

	size_t getHeightRequirement() const { return activeResolution->height; }
	size_t getWidthRequirement() const { return activeResolution->width; }
	float getContrastRequirement() const { return contrastRatio; }
	std::string getLanguage() const { return language; }
	std::filesystem::path getTessdataPath() const { return tessdataPath; }
	bool isValidLanguage() const { return validLanguage; }
};