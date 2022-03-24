#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>
#include "../include_private/AppSettings.h"
#include "../include_private/TextDetectionParams.h"
#include "../include_private/Guideline.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace tin {

	class Configuration {
	private:
		AppSettings appSettings;
		Guideline guideline;
		TextDetectionParams textDetectionParams;
		std::vector<double> rgbLookUp;
		
		
		void setDefaultGuideline();

		void setDefaultAppSettings();

		void setDefaultTextDetectionParams();

		template<typename T>
		static cv::Rect_<T> RectFromJson(json data);
	public:
		Configuration();
		Configuration(fs::path configPath);

		AppSettings* getAppSettings() { return &appSettings; }
		Guideline* getGuideline() { return &guideline; }
		TextDetectionParams* getTextDetectionParams() { return &textDetectionParams; }
		std::vector<double>* getRGBLookupTable() { return &rgbLookUp; }
	};

}