#include "Configuration.h"
#include "AppSettings.h"
#include "Guideline.h"
#include "TextDetectionParams.h"

namespace tin {
	Configuration::Configuration() {
		setDefaultAppSettings();
		setDefaultGuideline();
		setDefaultTextDetectionParams();
	}

	Configuration::Configuration(fs::path configPath) {
		std::ifstream configFile(configPath);
		if (configFile) {
			json config;
			configFile >> config;
			//Guidelines
			try {
				json guidelineJson = config["guideline"];
				std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines;
				for (auto it = guidelineJson["resolutions"].begin(); it != guidelineJson["resolutions"].end(); ++it)
				{
					ResolutionGuidelines a(it.value()["width"], it.value()["height"]);
					resolutionGuidelines[atoi(it.key().c_str())] = a;
				}

				std::unordered_map<int, ResolutionGuidelines> resolutionRecs;
				for (auto it = guidelineJson["resolutionsRecommendations"].begin(); it != guidelineJson["resolutionsRecommendations"].end(); ++it)
				{
					ResolutionGuidelines a(it.value()["width"], it.value()["height"]);
					resolutionRecs[atoi(it.key().c_str())] = a;
				}

				guideline = Guideline(guidelineJson["contrast"], guidelineJson["recommendedContrast"],
					guidelineJson["textBackgroundRadius"], resolutionGuidelines, resolutionRecs);

			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration guidelines" << std::endl;
				setDefaultGuideline();
			}
			//AppSettigns
			try {
				json settings = config["appSettings"];

				std::vector<cv::Rect2f> focus;
				for (auto it = settings["focusMask"].begin(); it != settings["focusMask"].end(); ++it)
				{
					focus.push_back(RectFromJson<float>(*it));
				}

				std::vector<cv::Rect2f> ignore;
				for (auto it = settings["ignoreMask"].begin(); it != settings["ignoreMask"].end(); ++it)
				{
					ignore.push_back(RectFromJson<float>(*it));
				}

				appSettings = AppSettings(settings["saveLuminanceMap"], settings["saveTextboxOutline"],
					settings["saveSeparateTexboxes"], settings["saveHistograms"], settings["saveRawTextboxOutline"],
					settings["saveLuminanceMasks"], settings["useTextRecognition"], settings["printValuesOnResults"],
					settings["resultsPath"], settings["debugInfoPath"]);
				if (!focus.empty()) {
					appSettings.setFocusMask(focus, ignore);
				}
			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration appSettings" << std::endl;
				setDefaultAppSettings();
			}
			//Text Detection
			try {
				json textDetection = config["textDetection"];
				json  mean = textDetection["detectionMean"];
				json merge = textDetection["mergeThreshold"];
				float degreeThreshold = textDetection["rotationThresholdDegrees"];
				std::pair<float, float> mergeThresh = std::make_pair(merge["x"], merge["y"]);
				textDetectionParams = TextDetectionParams(textDetection["detectionModel"], textDetection["confidence"],
					textDetection["nmsThreshold"], textDetection["detectionScale"],
					{ mean[0],mean[1] ,mean[2] }, mergeThresh, degreeThreshold * (CV_PI / 180));
			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration text detection params" << std::endl;
				setDefaultTextDetectionParams();
			}
			//text recognition
			try {
				json textRecognition = config["textRecognition"];
				json scale = textRecognition["scale"];
				json mean = textRecognition["mean"];
				json size = textRecognition["inputSize"];
				textRecognitionParams = TextRecognitionParams(textRecognition["recognitionModel"],
					textRecognition["decodeType"], textRecognition["vocabularyFile"],
					(double)scale["numerator"] / (double)scale["denominator"],
					{ mean[0],mean[1] ,mean[2] }, { size["width"],size["height"] });
			}
			catch (...) {
				BOOST_LOG_TRIVIAL(error) << "Malformed configuration: Text recognition params" << std::endl;
				setDefaultTextRecognitionParams();
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
			setDefaultGuideline();
			setDefaultTextDetectionParams();
			setDefaultAppSettings();
		}
	}

	void Configuration::setDefaultGuideline() {
		//If file is not found, error and set default values
		BOOST_LOG_TRIVIAL(error) << "Configuration file not found, falling back to default configuration\n";
		BOOST_LOG_TRIVIAL(error) << "Contrast ratio: 4.5" << std::endl;

		guideline = Guideline(4.5, 4.5, 100, { {1080,{4,28}} });
	}

	void Configuration::setDefaultAppSettings() {

		appSettings = AppSettings(true, true, false, false, false, true, true,
			true, "./", "./debugInfo");
	}

	void Configuration::setDefaultTextDetectionParams() {

		textDetectionParams = TextDetectionParams("frozen_east_text_detection.pb", 0.5, 0.4, 1.0, { 123.68, 116.78, 103.94 }, { 1.0,1.0 }, 0.17);
	}

	void Configuration::setDefaultTextRecognitionParams()
	{
		textRecognitionParams = TextRecognitionParams("crnn_cs.onnx", "CTC-greedy", "alphabet_94.txt", 1.0 / 127.5, { 127.5 ,127.5 ,127.5 }, { 100,32 });
	}

	template<typename T>
	cv::Rect_<T> Configuration::RectFromJson(json data) {
		return { data["x"], data["y"], data["w"], data["h"] };
	}
}
