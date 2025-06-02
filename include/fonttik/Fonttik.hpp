//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.
#pragma once

#include <filesystem>
#include <opencv2/core/mat.hpp>
namespace fs = std::filesystem;


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
	bool overAllPassSize = true;
	bool overAllPassContrast = true;
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

	std::pair<FrameResults, FrameResults> processFrame(Frame& frame, bool sizeByLine);

	std::pair<fs::path, fs::path> saveResults(Media& media, Results& results);

	std::pair<fs::path, fs::path> saveResultsToJson(fs::path outputPath, Results& results);

private:
	bool setResolutionGuideline(const Media& media);

	bool checkResolution(const cv::Size& mediaSize, const cv::Size& resolution);

	void calculateTextBoxLuminance(std::vector<TextBox>& textBoxes);

	void calculateTextMasks(std::vector<TextBox>& textBoxes);

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