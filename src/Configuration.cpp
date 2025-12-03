//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/Configuration.hpp"
#include <array>
#include "fonttik/Log.h"
#include "fonttik/Results.h"
#include <fstream>

namespace tik
{

Configuration::Configuration(const char* filePath)
{
	init(filePath);
}

void Configuration::init(const char* filePath)
{
	json config;
	try
	{
		std::ifstream file(filePath);
		config = json::parse(file, nullptr, true, true); //ignores comments on json files
	}
	catch (json::parse_error e)
	{
		LOG_CORE_ERROR(e.what());
		throw;
	}
	
	loadAppSettings(config["appSettings"]);

	sBgrValues = config["sRGBLinearizationValues"].get<std::vector<double>>();

	linearRGBToXYZJuddVosMatrix = loadMatrix(config["colorblindness"]["linearRGBToXYZJuddVosMatrix"]);
	XYZJuddVosToLMSMatrix = loadMatrix(config["colorblindness"]["XYZJuddVosToLMSMatrix"]);
	linearRGBToLMSMatrix = XYZJuddVosToLMSMatrix * linearRGBToXYZJuddVosMatrix;
	LMSToLinearRGBMatrix = linearRGBToLMSMatrix.inv();
	protanProjectionMatrix = loadMatrix(config["colorblindness"]["protanProjectionMatrix"]);
	deutanProjectionMatrix = loadMatrix(config["colorblindness"]["deutanProjectionMatrix"]);

	outlineColors.resize((int)ResultType::RESULTYPE_COUNT);
	outlineColors[(int)ResultType::PASS] = colorFromJson(config["appSettings"]["textboxOutlineColors"]["pass"]);
	outlineColors[(int)ResultType::WARNING] = colorFromJson(config["appSettings"]["textboxOutlineColors"]["warning"]);
	outlineColors[(int)ResultType::FAIL] = colorFromJson(config["appSettings"]["textboxOutlineColors"]["fail"]);
	outlineColors[(int)ResultType::UNRECOGNIZED] = colorFromJson(config["appSettings"]["textboxOutlineColors"]["unrecognized"]);

	loadMaskParams(config["appSettings"]);

	loadTextSizeParams(config["guideline"], config["appSettings"]);

	loadContrastRatioParams(config["guideline"]);

	loadTextDetectionParams(config["textDetection"]);
	
	std::string detectionBackend = config["appSettings"]["detectionBackend"];

	if (detectionBackend == "DB_EAST")
	{
		textDetectionBackend = DetectionBackend::DB_EAST;
		loadEASTParams(config["textDetection"]["DB_EAST"]);
	}
	else if (detectionBackend == "Rekognition")
	{
		textDetectionBackend = DetectionBackend::DB_Rekognition;
	}
	else
	{
		textDetectionBackend = DetectionBackend::DB_DiffBinarization;
		auto a = config["textDetection"];
		auto b = a["DB"];
		loadDiffBinarizationParams(b);
	}

	loadTextRecognitionParams(config["textRecognition"]);
}

void Configuration::loadAppSettings(const json& section)
{
	//Load appsettings
	int targetDPI = section["targetDPI"];
	std::string targetResolution = section["targetResolution"];
	bool saveTextboxOutline = section["saveTextboxOutline"];
	bool saveLogs = section["saveLogs"];
	bool printResultValues = section["printResultValues"];
	bool failsAsWarnings = section["failsAsWarnings"];
	bool useDPI = section["useDPI"];
	int analysisWaitSeconds = section["analysisWaitSeconds"];
	bool detectResolution = section["detectResolution"];
	bool sizeByLine = section["sizeByLine"];

	appSettings = { targetDPI, targetResolution, saveTextboxOutline, saveLogs, printResultValues, failsAsWarnings, useDPI, analysisWaitSeconds, detectResolution, sizeByLine};
}

void Configuration::loadMaskParams(const json& section)
{
	std::vector<cv::Rect2f> focusMask;
	for (auto it = section["focusMask"].begin(); it != section["focusMask"].end(); ++it)
	{
		focusMask.push_back(rectFromJson<float>(*it));
	}

	std::vector<cv::Rect2f> ignoreMask;
	for (auto it = section["ignoreMask"].begin(); it != section["ignoreMask"].end(); ++it)
	{
		ignoreMask.push_back(rectFromJson<float>(*it));
	}

	maskParams = { focusMask, ignoreMask };
}

void Configuration::loadTextSizeParams(const json& section, const json& section2)
{
	int heightPer100DPI = section["heightPer100DPI"];
	std::unordered_map<std::string, SizeGuidelines> resolutions = loadSizeGuidelines(section["resolutions"]);
	std::unordered_map<std::string, SizeGuidelines> resolutionsRecommendations = loadSizeGuidelines(section["resolutionsRecommendations"]);
	bool useTextRecognition = section2["useTextRecognition"];

	textSizeParams = { useTextRecognition, heightPer100DPI, resolutions, resolutionsRecommendations };
}

void Configuration::loadContrastRatioParams(const json& section)
{
	int textBackgroundRadius = section["textBackgroundRadius"];
	float contrast = section["contrast"];
	float recommendedContrast = section["recommendedContrast"];

	contrastRatioParams = { textBackgroundRadius, contrast, recommendedContrast };
}

void Configuration::loadTextDetectionParams(const json& section)
{
	float confidence = section["confidence"];
	std::pair<float, float> mergeThreshold = { section["mergeThreshold"]["x"], section["mergeThreshold"]["y"] };
	float rotationThresholdDegrees = section["rotationThresholdDegrees"];
	bool groupByCharacters = section["groupByCharacters"];
	std::string preferredBackend = section["preferredBackend"];
	std::string preferredTarget = section["preferredTarget"];
	textDetectionParams = { confidence, mergeThreshold, rotationThresholdDegrees, groupByCharacters, 
		textDetectionParams.getBackendParam(preferredBackend), textDetectionParams.getTargetParam(preferredTarget)};
}

void Configuration::loadTextRecognitionParams(const json& section)
{
	std::string recognitionModel = section["recognitionModel"];
	std::string decodeType = section["decodeType"];
	std::string vocabularyFile = section["vocabularyFile"];
	float scale = (float)section["scale"]["numerator"] / (float)section["scale"]["denominator"];
	std::array<double, 3> mean = { section["mean"][0], section["mean"][1], section["mean"][2] };
	std::pair<int, int> inputSize = { section["inputSize"]["width"], section["inputSize"]["height"] };

	textRecognitionParams = { recognitionModel, decodeType, vocabularyFile, scale, mean, inputSize };
}

void Configuration::loadEASTParams(const json& section)
{

	std::string detectionModel = section["detectionModel"];
	float nmsThreshold = section["nmsThreshold"];
	float detectionScale = section["detectionScale"];
	std::array<double, 3> detectionMean = { section["detectionMean"][0], section["detectionMean"][1], section["detectionMean"][2] };

	textDetectionParams.eastParams = {detectionModel, nmsThreshold, detectionScale, detectionMean};
}

void Configuration::loadDiffBinarizationParams(const json& section)
{
	std::string detectionModel = section["detectionModel"];
	float binaryThreshold = section["binaryThreshold"];
	float polygonThreshold = section["polygonThreshold"];
	int maxCandidates = section["maxCandidates"];
	float unclipRatio = section["unclipRatio"];
	float scale = section["scale"];
	std::array<double, 3> detectionMean = { section["detectionMean"][0], section["detectionMean"][1], section["detectionMean"][2] };
	std::array<int, 2> inputSize = { section["inputSize"][0], section["inputSize"][1] };

	textDetectionParams.dbParams = { detectionModel, binaryThreshold, polygonThreshold, maxCandidates, unclipRatio, scale, detectionMean, inputSize };
}

cv::Mat Configuration::loadMatrix(const json& section)
{
	std::vector<std::vector<double>> values = section.get<std::vector<std::vector<double>>>();
	cv::Mat matrix = cv::Mat(3, 3, CV_64F);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			matrix.at<double>(i, j) = values[i][j];
		}
	}
	return matrix;
}

template<typename T>
cv::Rect_<T> Configuration::rectFromJson(json data) 
{
	return { data["x"], data["y"], data["w"], data["h"] };
}

cv::Scalar Configuration::colorFromJson(json data) 
{
	cv::Scalar v;
	int size = data.size();
	for (int i = 0; i < 4; i++) 
	{
		//Fills empty positions with 1s, mainly for alpha channel
		v[i] = (i < size) ? static_cast<int>(data[i]) : 1;
	}
	//swaps from rgb to bgr
	int aux = v[2];
	v[2] = v[0];
	v[0] = aux;

	return v;
}

std::unordered_map<std::string, SizeGuidelines> Configuration::loadSizeGuidelines(const json& section)
{
	std::unordered_map<std::string, SizeGuidelines>guideLines;

	for (auto it = section.begin(); it != section.end(); ++it)
	{
		guideLines[it.key().c_str()] = SizeGuidelines{ it.value()["width"], it.value()["height"] };
	}

	return guideLines;
}

bool Configuration::setResolutionGuideline(const std::string& resolutionKey)
{
	std::unordered_map<std::string, SizeGuidelines>::iterator it;
	it = textSizeParams.resolutionGuidelines.find(resolutionKey);
	
	if (it != textSizeParams.resolutionGuidelines.end())
	{
		textSizeParams.activeGuideline = &it->second;
		return true;
	}
	else
	{
		LOG_CORE_ERROR("Resolution guideline not found for image width: {}", resolutionKey);
		return false;
	}
}

} //namespace tik