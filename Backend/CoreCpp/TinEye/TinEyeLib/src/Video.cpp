#include "Video.h"

namespace tin {
	Video::Video(fs::path filePath, cv::VideoCapture capture) : Media(filePath), videoCapture(capture)
	{
		if (videoCapture.isOpened()) {
			videoCapture >> imageMatrix;
		}
	}

	Video::~Video()
	{

		if (videoCapture.isOpened()) {
			videoCapture.release();
		}
	}

	bool Video::nextFrame()
	{
		imageMatrix.release();
		luminanceMap.release();

		if (videoCapture.isOpened()) {
			videoCapture >> imageMatrix;
			return !imageMatrix.empty();
		}
		else {
			return false;
		}
	}

	void Video::saveResultsOutlines(std::vector<std::vector<ResultBox>>& results, std::string fileName) {

	}
}

