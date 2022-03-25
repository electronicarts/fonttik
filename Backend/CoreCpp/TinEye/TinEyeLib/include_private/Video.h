#pragma once
#include "Media.h"

namespace tin {
	class Video : public Media {
		cv::VideoCapture videoCapture;

	public:
		Video(fs::path path, cv::VideoCapture capture);
		virtual ~Video();

		virtual bool nextFrame() override;

	};
}
