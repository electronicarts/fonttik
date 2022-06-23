#pragma once
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace tin {
	class AppSettings {
		bool dbgSaveLuminanceMap = false,
			dbgSaveTexboxOutline = false,
			dbgSaveSeparateTextboxes = false,
			dbgSaveHistograms = false,
			dbgSaveRawTextboxOutline = false,
			dbgSaveLuminanceMasks = false,
			useTextRecognition = false,
			dbgSaveLogs = false,
			printResultValues = false,
			useDPI = false;
		int targetDPI = 0,
			targetResolution = 0;
		fs::path resultsPath = "./", debugInfoPath = "./debug/";


		//All mask measurement range from 0 to 1, this will be scaled in functino of image size
		std::vector<cv::Rect2f> focusMasks = { {0,0,1,1} }; //If not provided, the whole screen will be the focus region
		std::vector<cv::Rect2f> ignoreMasks = {}; //Data inside here will be always ignored

		template<typename T>
		static cv::Rect_<T> RectFromJson(nlohmann::json data);
	public:
		AppSettings() :
			dbgSaveLuminanceMap(true),
			dbgSaveTexboxOutline(true), dbgSaveSeparateTextboxes(false),
			dbgSaveHistograms(false), dbgSaveRawTextboxOutline(false),
			dbgSaveLuminanceMasks(true), useTextRecognition(true),
			printResultValues(true), dbgSaveLogs(false),
			useDPI(false), targetDPI(0), targetResolution(0),
			resultsPath("./"), debugInfoPath("./debugInfo") {}

		void init(nlohmann::json settings);

		//AppSettigs
		bool saveLuminanceMasks() const { return dbgSaveLuminanceMasks; }
		bool saveLuminanceMap() const { return dbgSaveLuminanceMap; }
		bool saveTexboxOutline() const { return dbgSaveTexboxOutline; }
		bool saveSeparateTextboxes() const { return dbgSaveSeparateTextboxes; }
		bool saveHistograms() const { return dbgSaveHistograms; }
		bool saveRawTexboxOutline() const { return dbgSaveRawTextboxOutline; }
		bool textRecognitionActive() const { return useTextRecognition; }
		bool saveLogs() const { return dbgSaveLogs; }
		bool printValuesOnResults() const { return printResultValues; }
		fs::path getResultsPath() const { return resultsPath; }
		fs::path getDebugInfoPath() const { return debugInfoPath; }
		int getSpecifiedSize() const;
		bool usingDPI() const { return useDPI; }

		/// <summary>
		/// Applies a mask that sets to 0 al pixels set to be ignored
		/// </summary>
		/// <param name="focus">anything not contained here will be ignored</param>
		/// <param name="ignore">rects to be ignored</param>
		void setFocusMask(std::vector<cv::Rect2f> focus, std::vector<cv::Rect2f> ignore = {});

		cv::Mat calculateMask(int width, int height);
	};
}