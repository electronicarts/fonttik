//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <filesystem>
#include "Image.h"
#include "fonttik/Results.h"
#include "fonttik/Media.h"

namespace tik {
	
class Image : public Media {

public:
	Image(fs::path path, cv::Mat frame);
	virtual ~Image() {}

	virtual bool nextFrame() override { return false; };

	virtual void saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, const std::vector<cv::Scalar>& colors, bool saveNumbers) override;

	virtual Frame* getFrame() override;

protected:
	bool processed = false;
};

}