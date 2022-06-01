#pragma once
#include "Media.h"

namespace tin {
	class Video : public Media {
		cv::VideoCapture videoCapture;

		cv::Mat previousFrame;

		int frameCount = 0;
		static int framesToSkip;

		friend class VideoTests;
	public:
		Video(fs::path path, cv::VideoCapture capture);
		virtual ~Video();

		virtual bool nextFrame() override;

		virtual void saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, bool saveNumbers) override;

		/// <summary>
		/// Compares the given image matrix to the video's previousFrame.
		/// </summary>
		/// <param name="mat">Matrix to be compared two</param>
		/// <returns>True in case they are similar, false otherwise</returns>
		bool compareFramesSimilarity(cv::Mat& mat1, cv::Mat& mat2);

		//Sets how many frames should video processing skip between each frame analyzed
		static void setFramesToSkip(int numberOfFrames);

		//Returns current frame number identifier
		virtual int getFrameCount() override { return frameCount; }
	};
}
