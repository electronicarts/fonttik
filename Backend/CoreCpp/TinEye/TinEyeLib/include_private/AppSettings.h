#pragma once
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <unordered_map>

namespace fs = std::filesystem;

namespace tin {
	class AppSettings {
		bool dbgSaveLuminanceMap = false,
			dbgSaveTexboxOutline = false,
			dbgSaveSeparateTextboxes = false,
			dbgSaveHistograms = false,
			dbgSaveRawTextboxOutline = false,
			dbgSaveLuminanceMasks = false;
		fs::path resultsPath = "./", debugInfoPath = "./debug/";


		//All mask measurement range from 0 to 1, this will be scaled in functino of image size
		std::vector<cv::Rect2f> focusMasks = { {0,0,1,1} }; //If not provided, the whole screen will be the focus region
		std::vector<cv::Rect2f> ignoreMasks = {}; //Data inside here will be always ignored

	public:
		AppSettings() {};
		AppSettings(bool saveLum, bool saveTextbox, bool saveSeparateTexbox, bool saveHist, bool saveRawTextbox, bool saveLumMasks,
			fs::path resultsPath, fs::path dbgInfoPath) :dbgSaveLuminanceMap(saveLum),
			dbgSaveTexboxOutline(saveTextbox), dbgSaveRawTextboxOutline(saveRawTextbox),
			dbgSaveSeparateTextboxes(saveSeparateTexbox), dbgSaveHistograms(saveHist),
			dbgSaveLuminanceMasks(saveLumMasks),
			resultsPath(resultsPath), debugInfoPath(dbgInfoPath) {}

		//AppSettigs
		bool saveLuminanceMasks() const { return dbgSaveLuminanceMasks; }
		bool saveLuminanceMap() const { return dbgSaveLuminanceMap; }
		bool saveTexboxOutline() const { return dbgSaveTexboxOutline; }
		bool saveSeparateTextboxes() const { return dbgSaveSeparateTextboxes; }
		bool saveHistograms() const { return dbgSaveHistograms; }
		bool saveRawTexboxOutline() const { return dbgSaveRawTextboxOutline; }
		fs::path getResultsPath() const { return resultsPath; }
		fs::path getDebugInfoPath() const { return debugInfoPath; }

		void setFocusMask(std::vector<cv::Rect2f> focus, std::vector<cv::Rect2f> ignore = {});

		cv::Mat calculateMask(int width, int height);
	};
}