#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

class AppSettings;
class Guideline;
class TextDetectionParams;


class Configuration {
private:
	AppSettings* appSettings = nullptr;
	Guideline* guideline = nullptr;
	TextDetectionParams* textDetectionParams = nullptr;

	void setDefaultGuideline();

	void setDefaultAppSettings();

	void setDefaultTextDetectionParams();

	template<typename T>
	static cv::Rect_<T> RectFromJson(json data);
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
		if (textDetectionParams != nullptr) {
			delete textDetectionParams;
		}
		
		appSettings = nullptr;
		guideline = nullptr;
		textDetectionParams = nullptr;
	}

	AppSettings* getAppSettings() const { return appSettings; }
	Guideline* getGuideline() const { return guideline; }
	TextDetectionParams* getTextDetectionParams() const { return textDetectionParams; }
};