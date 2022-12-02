//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "Media.h"

namespace tik {
	
class Video : public Media {

public:
	Video(fs::path path, cv::VideoCapture capture);
	virtual ~Video();

	virtual Frame* getFrame() override;

	virtual bool nextFrame() override;

	virtual void saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, const std::vector<cv::Scalar>& colors, bool saveNumbers) override;

	/// <summary>
	/// Compares the given image matrix to the video's previousFrame.
	/// </summary>
	/// <param name="mat">Matrix to be compared two</param>
	/// <returns>True in case they are similar, false otherwise</returns>
	bool compareFramesSimilarity(cv::Mat& mat1, cv::Mat& mat2);

	//Sets how many frames should video processing skip between each frame analyzed
	static void setFramesToSkip(int numberOfFrames);

	//Sets the interval of frames where an image output should be created for a failing frame when analyzing video
	static void setFrameOutputInterval(int numberOfFrames);

	//Returns current frame number identifier
	virtual int getFrameCount() override { return frameCount; }
private:
	cv::VideoCapture videoCapture;

	cv::Mat previousFrame;

	int frameCount = 0;
	static int framesToSkip;
	static int frameOutputInterval;

	friend class VideoTests;
};

}
