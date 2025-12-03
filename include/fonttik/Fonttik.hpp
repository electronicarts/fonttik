//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.
#pragma once

#include <filesystem>
#include <opencv2/core/mat.hpp>
namespace fs = std::filesystem;
#include "Results.h"
#include "../src/ColorblindFilters.hpp"

namespace tik
{

class Configuration;
class Media;
class Results;
class Frame;
class ITextboxDetection;
class ITextBoxRecognition;
class IChecker;
class TextBox;
struct FrameResults;

struct AsyncResults 
{
	fs::path pathToSizeResult;
	fs::path pathToContrastResult;
	fs::path pathToJSONSizeResult;
	fs::path pathToJSONContrastResult;

	fs::path pathToProtanResult;
	fs::path pathToDeutanResult;
	fs::path pathToTritanResult;
	fs::path pathToGrayscaleResult;

	bool overAllPassSize = true;
	tik::ResultType overAllResultSize = PASS;
	bool overAllPassContrast = true;
	tik::ResultType overAllResultContrast = PASS;
	std::vector<bool> overallPassColorblind = { true, true, true, true };
	std::vector<ResultType> overallResultColorblind = { PASS, PASS, PASS, PASS };
};



class Fonttik
{
public:
	Fonttik() {};
	Fonttik(Configuration* config) { init(config); };
	~Fonttik();

	void init(Configuration* config);

	AsyncResults processMediaAsync(Media& media);

	Results processMedia(Media& media);

	std::pair<FrameResults, FrameResults> processFrame(Frame& frame, std::vector<Frame> colorblindFrames, bool sizeByLine);
	
	std::pair<fs::path, fs::path> saveResults(Media& media, Results& results);

	std::pair<fs::path, fs::path> saveResultsToJson(fs::path outputPath, Results& results);

	ColorblindFilters* colorblindFilters = nullptr;

private:
	bool setResolutionGuideline(const Media& media);

	bool checkResolution(const cv::Size& mediaSize, const cv::Size& resolution);

	void calculateTextBoxLuminance(std::vector<TextBox>& textBoxes);

	void calculateTextMasks(std::vector<TextBox>& textBoxes);

	std::vector<std::vector<TextBox>> createColorblindTextBoxes(std::vector<Frame> colorblindFrames, std::vector<TextBox> words);

	/// <summary>
	/// Sets the recognized text in the contrast results
	/// </summary>
	void setTextInContrastResults(const FrameResults& sizeResults, FrameResults& contrastResults);

	Configuration* configuration = nullptr;
	ITextboxDetection* textBoxDetection = nullptr;
	ITextBoxRecognition* textBoxRecognition = nullptr;
	IChecker* contrastChecker = nullptr;
	IChecker* sizeChecker = nullptr;
	const int MAX_LEEWAY = 100; //Maximum leeway for the resolution when detecting the media resolution
	const cv::Size RESOLUTION_1080p = cv::Size(1920, 1080);
	const cv::Size RESOLUTION_720p = cv::Size(1280, 720);
	const cv::Size RESOLUTION_STEAM_DECK = cv::Size(1280, 720);
	const cv::Size RESOLUTION_2160p = cv::Size(3840, 2160); 
};

}