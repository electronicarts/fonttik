//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.
#pragma once

#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <opencv2/core/types.hpp>

namespace tik 
{

struct AppSettings
{
	int targetDPI;
	std::string targetResolution; //name of the resolution for size guidelines, if detectResolution is false, this will be used [720,1080,SeamDeck...]
	bool saveTextboxOutline;
	bool saveLogs;
	bool printResultValues;
	bool failsAsWarnings;
	bool useDPI;
	int analysisWaitSeconds;
	bool detectResolution;
	bool sizeByLine;
};

struct MaskParams
{
	//All mask measurement range from 0 to 1, this will be scaled in functino of image size
	std::vector<cv::Rect2f> focusMasks = { {0,0,1,1} }; //If not provided, the whole screen will be the focus region
	std::vector<cv::Rect2f> ignoreMasks; //Data inside here will be always ignored
};

struct SizeGuidelines {
	size_t width;
	size_t height;
	SizeGuidelines() :width(0), height(0) {}
	SizeGuidelines(size_t w, size_t h)
		:width(w), height(h) {}
};

struct TextSizeParams
{
	bool useTextRecognition;
	int heightPer100DPI;
	std::unordered_map<std::string, SizeGuidelines> resolutionGuidelines;
	std::unordered_map<std::string, SizeGuidelines> resolutionRecommendations;
	std::unordered_map<std::string, SizeGuidelines> dpiGuidelines;
	SizeGuidelines* activeGuideline = nullptr;
	SizeGuidelines* activeRecommendation = nullptr;

};

struct ContrastRatioParams
{
	int textBackgroundRadius;
	float contrastRatio;
	float contrastRatioRecommendation;
};

struct EASTDetectionParams
{
	std::string detectionModel;
	float nonMaxSuprresionThreshold; //Non maximum supresison threshold
	double detectionScale; //Scales pixel individually after mean substraction
	std::array<double, 3> detectionMean;//This values will be substracted from the corresponding channel
};

struct DBDetectionParams
{
	std::string detectionModel = "DB_IC15_resnet50.onnx";
	float binThresh = 0.3;
	float polyThresh = 0.5;
	int maxCandidates = 200;
	double unclipRatio = 2.0; //Equivalent to non max suppression
	float scale = 1.0 / 255;
	std::array<double, 3> mean = { 123.68, 116.78, 103.94 };//This values will be substracted from the corresponding channel
	std::array<int, 2> inputSize = { 736,736 };//This values will be substracted from the corresponding channel
};

struct TextDetectionParams
{
	enum class PreferredBackend { DEFAULT = 0, CUDA = 5 }; // equal to cv::dnn::Backend
	enum class PreferredTarget { CPU = 0, OPENCL = 1, CUDA = 6 }; // equal to cv::dnn::Target

	PreferredBackend getBackendParam(std::string param)
	{
		return param == "CUDA" ? PreferredBackend::CUDA : PreferredBackend::DEFAULT;
	}

	PreferredTarget getTargetParam(std::string param)
	{
		return param == "CUDA" ? PreferredTarget::CUDA : (param == "OPENCL" ? PreferredTarget::OPENCL : PreferredTarget::CPU);
	}

	float confidenceThreshold; 
	std::pair<float, float> mergeThreshold; //If overlap in both axes surpasses this value, textboxes will be merged
	float rotationThresholdRadians; //Text that excedes this inclination will be ignored (not part of the HUD)
	bool groupByCharacters;
	PreferredBackend preferredBackend = PreferredBackend::DEFAULT;
	PreferredTarget preferredTarget = PreferredTarget::CPU;
	EASTDetectionParams eastParams;
	DBDetectionParams dbParams;
};

struct TextRecognitionParams
{
	std::string recognitionModel;
	std::string decodeType;
	std::string vocabularyFilePath;
	double scale;
	std::array<double, 3> mean;
	std::pair<int, int> size;
};

}