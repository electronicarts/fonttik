#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <filesystem>
#include "Image.h"
#include "Results.h"
#include "Media.h"

namespace tin {
	class Image : public Media {
	protected:

	public:
		Image(fs::path path, cv::Mat frame);
		virtual ~Image() {}

		virtual bool nextFrame() override { return false; }

		virtual void saveResultsOutlines(std::vector<std::vector<ResultBox>>& results, std::string fileName) override;
	};
}