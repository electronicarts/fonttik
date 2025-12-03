//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "ConfigurationParams.hpp"
#include <opencv2/core/mat.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace tik
{

class Configuration
{
public:
	void init(const char* filePath);

	Configuration() {};
	Configuration(const char* filePath);
	~Configuration() {};

	enum class DetectionBackend { DB_EAST, DB_DiffBinarization, DB_Rekognition };

	inline const AppSettings& getAppSettings() const { return appSettings; }
	inline const MaskParams& getMaskParams() const { return maskParams; }
	inline const TextDetectionParams& getTextDetectionParams() const { return textDetectionParams; }
	inline const TextRecognitionParams& getTextRecognitionParams() const { return textRecognitionParams; }
	inline const DetectionBackend& getTextDetectionBackend() const { return textDetectionBackend; }
	inline const ContrastRatioParams& getContrastRatioParams() const { return contrastRatioParams; }
	inline const TextSizeParams& getTextSizeParams() const { return textSizeParams; }
	inline const std::vector<double>& getSbgrValues() const { return sBgrValues; }
	inline const cv::Mat& getLinearRGBToLMSMatrix() const { return linearRGBToLMSMatrix; }
	inline const cv::Mat& getXYZJuddVosToLMSMatrix() const { return XYZJuddVosToLMSMatrix; }
	inline const cv::Mat& getLMSToLinearRGBMatrix() const { return LMSToLinearRGBMatrix; }
	inline const cv::Mat& getProtanProjectionMatrix() const { return protanProjectionMatrix; }
	inline const cv::Mat& getDeutanProjectionMatrix() const { return deutanProjectionMatrix; }
	const std::vector<cv::Scalar> getOutlineColors() const { return outlineColors; }

	inline void setAnalysisWaitSeconds(const int& aws) { appSettings.analysisWaitSeconds = aws; }
	bool setResolutionGuideline(const std::string& imgWidth);
	/// <summary>
	/// Disables automatic resolution detection and sets a target resolution to use the guidelines
	/// </summary>
	/// <param name="v"></param>
	inline void setTargetResolution(std::string resolution) { appSettings.detectResolution = false; appSettings.targetResolution = resolution; }
	inline void setMaskParams(const MaskParams& mp) { maskParams = mp; }
	inline void setMaskParams(const std::vector<cv::Rect2f>& focusMasks, const std::vector<cv::Rect2f>& ignoreMasks) { 
		if (focusMasks.empty())
		{
			maskParams.focusMasks = { cv::Rect2f{ 0, 0, 1, 1 } };
		}
		else {
			maskParams.focusMasks = focusMasks;
		}
		maskParams.ignoreMasks = ignoreMasks;
	}
	inline void setUseOcr(const bool useOcr) { textSizeParams.useTextRecognition = useOcr; }
	inline void setTreatFailsAsWarnings(const bool failsAsWarnings) { appSettings.failsAsWarnings = failsAsWarnings; }
	inline void setContrastRatio(const float cr) { contrastRatioParams.contrastRatio = cr; }
	inline void setSizeGuideline(const std::string& height, const SizeGuidelines& sg) { textSizeParams.resolutionGuidelines[height] = sg; }
	inline void setSizeByLine(bool sizeByLine) { appSettings.sizeByLine = sizeByLine; }


private:
	template<typename T>
	static cv::Rect_<T> rectFromJson(json data);

	void loadAppSettings(const json& section);
	void loadMaskParams(const json& section);
	void loadTextDetectionParams(const json& section);
	void loadTextRecognitionParams(const json& section);
	void loadContrastRatioParams(const json& section);
	void loadTextSizeParams(const json& section, const json& section2);
	void loadEASTParams(const json& section);
	void loadDiffBinarizationParams(const json& section);
	cv::Mat loadMatrix(const json& section);
	std::unordered_map<std::string, SizeGuidelines> loadSizeGuidelines(const json& section);

	/// <summary>
	/// Loads a color from a JSON, expects RGB because is the most common format for users
	/// The output is BGR because of OpenCVs implementation
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	static cv::Scalar colorFromJson(nlohmann::json data);


	DetectionBackend textDetectionBackend;
	AppSettings appSettings;
	MaskParams maskParams;
	TextDetectionParams textDetectionParams;
	TextRecognitionParams textRecognitionParams;
	ContrastRatioParams contrastRatioParams;
	TextSizeParams textSizeParams;

	cv::Mat linearRGBToXYZJuddVosMatrix;
	cv::Mat XYZJuddVosToLMSMatrix;
	cv::Mat linearRGBToLMSMatrix;
	cv::Mat LMSToLinearRGBMatrix;
	cv::Mat protanProjectionMatrix;
	cv::Mat deutanProjectionMatrix;
	std::vector<double> sBgrValues;
	std::vector<cv::Scalar> outlineColors;
};

}