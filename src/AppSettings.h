//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "fonttik/Results.h"

namespace fs = std::filesystem;

namespace tik {
	
class AppSettings {

public:
	enum DetectionBackend {EAST, DB};

	AppSettings() :
		detectionBackend(DetectionBackend::EAST),
		dbgSaveLuminanceMap(true),
		dbgSaveTexboxOutline(true), dbgSaveSeparateTextboxes(false),
		dbgSaveHistograms(false), dbgSaveRawTextboxOutline(false),
		dbgSaveLuminanceMasks(true), useTextRecognition(true),
		printResultValues(true), dbgSaveLogs(false),
		useDPI(false), targetDPI(0), targetResolution(0) {}

	void init(nlohmann::json settings);

	//AppSettigs
	DetectionBackend getDetectionBackend() const { return detectionBackend; }
	bool saveLuminanceMasks() const { return dbgSaveLuminanceMasks; }
	bool saveLuminanceMap() const { return dbgSaveLuminanceMap; }
	bool saveTexboxOutline() const { return dbgSaveTexboxOutline; }
	bool saveSeparateTextboxes() const { return dbgSaveSeparateTextboxes; }
	bool saveHistograms() const { return dbgSaveHistograms; }
	bool saveRawTexboxOutline() const { return dbgSaveRawTextboxOutline; }
	bool textRecognitionActive() const { return useTextRecognition; }
	bool saveLogs() const { return dbgSaveLogs; }
	bool printValuesOnResults() const { return printResultValues; }
	int getSpecifiedSize() const;
	bool usingDPI() const { return useDPI; }
	const auto& getColors() { return outlineColors; }

	/// <summary>
	/// Applies a mask that sets to 0 al pixels set to be ignored
	/// </summary>
	/// <param name="focus">anything not contained here will be ignored</param>
	/// <param name="ignore">rects to be ignored</param>
	void setFocusMask(std::vector<cv::Rect2f> focus, std::vector<cv::Rect2f> ignore = {});

	cv::Mat calculateMask(int width, int height);

private:
	DetectionBackend detectionBackend = DetectionBackend::EAST; //Text detection can use EAST or DB

	int targetDPI = 0,
		targetResolution = 0;
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

	std::vector<cv::Scalar> outlineColors = std::vector<cv::Scalar>(ResultType::RESULTYPE_COUNT, cv::Scalar(1.0));


	//All mask measurement range from 0 to 1, this will be scaled in functino of image size
	std::vector<cv::Rect2f> focusMasks = { {0,0,1,1} }; //If not provided, the whole screen will be the focus region
	std::vector<cv::Rect2f> ignoreMasks = {}; //Data inside here will be always ignored

	template<typename T>
	static cv::Rect_<T> RectFromJson(nlohmann::json data);

	/// <summary>
	/// Loads a color from a JSON, expects RGB because is the most common format for users
	/// The output is BGR because of OpenCVs implementation
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	static cv::Scalar ColorFromJson(nlohmann::json data);
};

}