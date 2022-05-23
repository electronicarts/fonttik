#pragma once
#include "Media.h"

namespace tin {
	class Video : public Media {
		cv::VideoCapture videoCapture;

		cv::Mat previousFrame;

		int frameCount = 0;

		friend class VideoTests;
	public:
		Video(fs::path path, cv::VideoCapture capture);
		virtual ~Video();

		virtual bool nextFrame() override;

		virtual void saveResultsOutlines(std::vector<std::vector<ResultBox>>& results, fs::path path, bool saveNumbers) override;

		/// <summary>
		/// Compares the given image matrix to the video's previousFrame.
		/// </summary>
		/// <param name="mat">Matrix to be compared two</param>
		/// <returns>True in case they are similar, false otherwise</returns>
		bool compareFramesSimilarity(cv::Mat& mat1, cv::Mat& mat2);
	};
}
