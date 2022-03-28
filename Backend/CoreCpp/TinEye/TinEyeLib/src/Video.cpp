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
		//Create output path
		fs::path outputPath = path.parent_path() / (path.filename().string() + "_output");

		if (!fs::is_directory(outputPath) || !fs::exists(outputPath)) {
			fs::create_directory(outputPath);
		}

		//Set source video to first frame
		videoCapture.set(cv::CAP_PROP_POS_FRAMES, 0);

		//Get size from input video
		cv::Size size = cv::Size((int)videoCapture.get(cv::CAP_PROP_FRAME_WIDTH), (int)videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

		//Create output video
		cv::VideoWriter outputVideo((outputPath / (fileName + ".mp4")).string(), videoCapture.get(cv::CAP_PROP_FOURCC),
			videoCapture.get(cv::CAP_PROP_FRAME_COUNT), size, true);

		//Iterate through every video frame and result, if either is empty exit
		int i = 0;
		cv::Mat frameMat, frameCopy;
		videoCapture >> frameMat;
		while (!frameMat.empty() && i < results.size()) {
			//clone frame for editing
			frameCopy = frameMat.clone();

			for (ResultBox& box : results[i]) {
				cv::Scalar color = box.getResultColor();
				highlightBox(box.x, box.y, box.x + box.width, box.y + box.height, color, frameCopy, 2);
			}

			//output frame to video
			outputVideo << frameCopy;
			i++;
			videoCapture >> frameMat;
		}
	}
}

