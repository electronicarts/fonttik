#pragma once
#include "Media.h"

namespace tin {
	class Video : public Media {
		cv::VideoCapture videoCapture;

		int frameCount = 0;
	public:
		Video(fs::path path, cv::VideoCapture capture);
		virtual ~Video();

		virtual bool nextFrame() override;

		virtual void saveResultsOutlines(std::vector<std::vector<ResultBox>>& results, std::string fileName) override;
	};
}
