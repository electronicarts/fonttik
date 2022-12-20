//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/Media.h"
#include "fonttik/Fonttik.h"
#include "Video.h"
#include "Image.h"
#include "fonttik/Frame.h"

namespace tik {
	Media* Media::CreateMedia(fs::path path) {
		Media* media = nullptr;
	
		//Attempt to open file as image, if not possible then try as video
		cv::Mat imgMat = cv::imread(path.string(), cv::IMREAD_COLOR);
		if (!imgMat.empty()) {
			media = new Image(path, imgMat);
		}
		else {
			cv::VideoCapture capture(path.string());
			if (capture.isOpened()) {
				media = new Video(path, capture);
			}
		}

		return media;
	}

	Media::~Media() {
		if (!imageMatrix.empty()) {
			imageMatrix.release();
		}
	}

	void Media::saveOutputData(cv::Mat data, fs::path path) {
		cv::imwrite(path.string(), data);
	}

	fs::path Media::getOutputPath() {
		std::string out= path.parent_path().string() +"/" + (path.stem().string() + "_output");
		fs::path outputPath(out);

		if (!fs::is_directory(outputPath) || !fs::exists(outputPath)) {
			fs::create_directory(outputPath);
		}

		return outputPath;
	}
}