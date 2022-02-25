#pragma once
#include <fstream>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
namespace fs = std::filesystem;

class AppSettings;
class Guideline;



class Configuration {
private:
	AppSettings* appSettings = nullptr;
	Guideline* guideline = nullptr;

	void setDefaultGuideline();

	void setDefaultAppSettings();
public:
	Configuration();
	Configuration(fs::path configPath);
	~Configuration() {
		if (appSettings != nullptr) {
			delete appSettings;
		}
		if (guideline != nullptr) {
			delete guideline;
		}
		
		appSettings = nullptr;
		guideline = nullptr;
	}

	AppSettings* getAppSettings() const { return appSettings; }
	Guideline* getGuideline() const { return guideline; }
};