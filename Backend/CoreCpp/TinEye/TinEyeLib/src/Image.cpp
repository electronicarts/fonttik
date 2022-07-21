//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "Image.h"
#include <fstream>
#include "Media.h"
#include "Frame.h"

namespace fs = std::filesystem;

namespace tin {

	Image::Image(fs::path filePath, cv::Mat imgMat) :Media(filePath) {
		imageMatrix = imgMat;
	}

	void Image::saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, const std::vector<cv::Scalar>& colors, bool saveNumbers) {
		cv::Mat highlights = imageMatrix.clone();

		for (ResultBox& box : results.back().results) {
			cv::Scalar color = colors[box.type];
			Frame::highlightBox(box.x, box.y, box.x + box.width, box.y + box.height, color, highlights, 2);
		}

		//Add measurements after outline so it doesn't cover the numbers
		if (saveNumbers) {
			for (ResultBox& box : results.back().results) {
				Frame::putResultBoxValues(highlights, box, (path.stem() == "contrastChecks") ? 1 : 0); //Only add decimals with contrast checks
			}
		}

		saveOutputData(highlights, path.string() + ".png");
	}

	Frame* Image::getFrame() {
		Frame* frame = (!processed) ? new Frame(this,0,imageMatrix) : nullptr;
		processed = true;

		return frame;
	}
}