//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "Results.h"
#include "fonttik/Frame.hpp"

#include <rigtorp/SPSCQueue.h>
#include <vector>
#include <string>
#include <filesystem>
#include <variant>

namespace fs = std::filesystem;

namespace tik
{
class Frame;
struct MaskParams;

struct FrameResult
{
	FrameResults size;
	FrameResults contrast;
	int frameID;
	std::string timeStamp = "";
};
class Media
{

public:
	struct SaveResultProperties
	{
		std::vector<FrameResults> results;
		std::vector<cv::Scalar> colors;
		const fs::path path;
		const bool saveNumbers;
	};

	virtual ~Media() {};

	//Factory Method that creates a video or an image depending on the file
	//returns nullptr in case of invalid file
	static Media* createMedia(std::string mediaSource);

	virtual void calculateMask(const MaskParams& maskParams);

	/// <summary>
	/// Sets the next frame to analyse
	/// </summary>
	/// <returns>True if the frame was loaded correctly</returns>
	virtual bool loadFrame() = 0;

	/// <summary>
	/// Returns the current loaded frame pending to be analysed
	/// </summary>
	virtual Frame getFrame() = 0;

	fs::path getPath() {
		return fs::path{mediaSource};
	};

	//Generates outlines for the image's results and saves them. Returns the paths to the saved files (size results and then contrast)
	virtual std::pair<fs::path, fs::path>saveResultsOutlines(const SaveResultProperties& sizeResultProperties,
		const SaveResultProperties& contrastResultProperties) = 0;

	virtual void saveResultsOutlinesAsync(const SaveResultProperties& sizeResultProperties, const SaveResultProperties& contrastResultProperties,
		rigtorp::SPSCQueue<FrameResult>& queue, std::atomic<bool>& done) = 0;


	//Saves the data in the image sub folder
	void saveOutputData(cv::Mat data, fs::path path);

	//Returns output path and if it doesn't exist creates it
	fs::path getOutputPath();

	virtual std::string getExtension() = 0;

	virtual void setAnalysisWaitSeconds(int aws) {};

	const cv::Size& getImageSize() const { return imageSize; }

protected:

	Media(std::string mediaPath) : mediaSource{mediaPath}, frameIndex{ 0 } {}
	
	cv::Mat mask; //result of the calculation of the focus and ignore masks
	cv::Size imageSize; //video frame or image size

	std::string mediaSource{};

	int frameIndex; //current frame position 
};

}