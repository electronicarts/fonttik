#include "Image.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include "Media.h"

namespace fs = std::filesystem;

namespace tin {

	Image::Image(fs::path filePath, cv::Mat imgMat) :Media(filePath){
		imageMatrix = imgMat;
	}

	void Image::saveResultsOutlines(std::vector<std::vector<ResultBox>>& results, std::string fileName) {
		cv::Mat highlights = imageMatrix.clone();

		for (ResultBox& box : results.back()) {
			cv::Scalar color;
			switch (box.type) {
			case ResultType::PASS:
				color = cv::Scalar(0, 255, 0);
				break;
			case ResultType::FAIL:
				color = cv::Scalar(0, 0, 255);
				break;
			case ResultType::UNRECOGNIZED:
				color = cv::Scalar(255, 0, 0);
				break;
			case ResultType::WARNING:
				color = cv::Scalar(0, 170, 255);
				break;
			}
			highlightBox(box.x, box.y, box.x + box.width, box.y + box.height, color, highlights, 2);
		}
		saveOutputData(highlights, fileName);
	}
}