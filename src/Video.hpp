//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "fonttik/Media.hpp"
#include <atomic>

namespace tik
{
class Frame;

class Video : public Media
{
public: 
	Video(std::string mediaPath);
	virtual ~Video() = default;

	virtual bool loadFrame() override;

	virtual Frame getFrame() override;

	virtual std::pair<fs::path, fs::path>saveResultsOutlines(const SaveResultProperties& sizeResultProperties, 
		const SaveResultProperties& contrastResultProperties) override;

	virtual void saveResultsOutlinesAsync(const SaveResultProperties& sizeResultProperties,
		const SaveResultProperties& contrastResultProperties, rigtorp::SPSCQueue<FrameResult>& queue, std::atomic<bool>& done) override;

	virtual std::string getExtension() override { return ".mp4"; }

	//Sets how many frames should video processing skip between each frame analyzed by X amount of seconds
	virtual void setAnalysisWaitSeconds(int aws) override;
	

	/// <summary>
	/// Compares the given image matrix to the video's previousFrame.
	/// </summary>
	/// <param name="mat">Matrix to be compared two</param>
	/// <returns>True in case they are similar, false otherwise</returns>
	bool compareFramesSimilarity(const cv::Mat& a, const cv::Mat& b);

	///Dangerous function, use with care!!
	///Skips next frame and compare similarity logic. Currently only used for unit tests.
	cv::Mat _GetNextFrame() { cv::Mat ret; video >> ret; return ret; };

private:
	cv::VideoWriter CreateOutputVideoWritter(const fs::path& outputPath, cv::Size outputSize, double FPS);

	void storeResultsInJSON(const std::vector<ResultBox>& res, int id, const std::string& timeStamp, std::ofstream& out);

	cv::VideoCapture video;
	cv::Mat currentFrame;
	cv::Mat previousFrame;

	int msTimeStamp;
	static int framesToSkip;
	static int fps;
};

}