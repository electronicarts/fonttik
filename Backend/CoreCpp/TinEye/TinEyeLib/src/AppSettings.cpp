#include "AppSettings.h"
#include <tuple>
#include <opencv2/imgcodecs.hpp>
#include "Video.h"

namespace tin {
	void AppSettings::init(nlohmann::json settings) {
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
		if (!focus.empty()) {
			setFocusMask(focus, ignore);
		}

		dbgSaveLuminanceMap = settings["saveLuminanceMap"];
		dbgSaveTexboxOutline = settings["saveTextboxOutline"];
		dbgSaveRawTextboxOutline = settings["saveSeparateTexboxes"];
		dbgSaveSeparateTextboxes = settings["saveHistograms"];
		dbgSaveHistograms = settings["saveRawTextboxOutline"];
		dbgSaveLuminanceMasks = settings["saveLuminanceMasks"];
		useTextRecognition = settings["useTextRecognition"];
		printResultValues = settings["printValuesOnResults"];
		dbgSaveLogs = settings["saveLogs"];
		resultsPath = std::string(settings["resultsPath"]);
		debugInfoPath = std::string(settings["debugInfoPath"]);
		useDPI = settings["useDPI"];
		targetDPI = settings["targetDPI"];
		targetResolution = settings["targetResolution"];

		int framesToSkip = settings["videoFramesToSkip"];
		int videoFrameOutputInterval = settings["videoImageOutputInterval"];
		Video::setFramesToSkip(framesToSkip);
		Video::setFrameOutputInterval(videoFrameOutputInterval);
	}

	void AppSettings::setFocusMask(std::vector<cv::Rect2f> focus, std::vector<cv::Rect2f> ignore) {
		if (!focus.empty()) {
			focusMasks = focus;
		}
		else {
			focusMasks = { {0,0,1,1} }; //If theres no focus region, we will analyse everything
		}

		ignoreMasks = ignore;
	}

	cv::Mat AppSettings::calculateMask(int width, int height) {
		cv::Mat mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

		for (cv::Rect2f rect : focusMasks) {
			cv::Rect absRect(rect.x * width, rect.y * height,
				rect.width * width, rect.height * height);

			cv::Mat subMatrix = mat(absRect);
			subMatrix.setTo(cv::Scalar(255, 255, 255));
		}

		//Ignore masks will be ignored even if inside focus regions
		for (cv::Rect2f rect : ignoreMasks) {
			cv::Rect absRect(rect.x * width, rect.y * height,
				rect.width * width, rect.height * height);

			cv::Mat subMatrix = mat(absRect);
			subMatrix.setTo(cv::Scalar(0, 0, 0));
		}
		return mat;
	}

	int AppSettings::getSpecifiedSize() const {
		return (useDPI) ? targetDPI : targetResolution;
	}

	template<typename T>
	cv::Rect_<T> AppSettings::RectFromJson(nlohmann::json data) {
		return { data["x"], data["y"], data["w"], data["h"] };
	}
}