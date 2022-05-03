#include "Video.h"
#include "boost/log/trivial.hpp"

namespace tin {
	Video::Video(fs::path filePath, cv::VideoCapture capture) : Media(filePath), videoCapture(capture)
	{
		if (videoCapture.isOpened()) {
			frameCount = 0;
			videoCapture >> imageMatrix;
			previousFrame = imageMatrix;
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
			BOOST_LOG_TRIVIAL(trace) << "Processing video frame " << ++frameCount << std::endl;
			videoCapture >> imageMatrix;

			while (!imageMatrix.empty() && compareFramesSimilarity(imageMatrix)) {
				//Keep loading new frames until video is over or we find one that is 
				//sufficiently different from the previously processed one
				videoCapture >> imageMatrix;
				++frameCount;
			}

			previousFrame = imageMatrix;
			return !imageMatrix.empty();
		}
		else {
			return false;
		}
	}

	void Video::saveResultsOutlines(std::vector<std::vector<ResultBox>>& results, std::string fileName, bool saveNumbers) {
		//Create output path
		fs::path outputPath = getOutputPath();

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

			//Add measurements after boxes so boxes don't cover the numbers
			if (saveNumbers) {
				for (ResultBox& box : results.back()) {
					putResultBoxValues(frameCopy, box, (fileName == "contrastChecks") ? 1 : 0); //Only add decimals with contrast checks
				}
			}

			//output frame to video
			outputVideo << frameCopy;
			i++;
			videoCapture >> frameMat;
		}
	}

	bool Video::compareFramesSimilarity(cv::Mat& mat) {
		cv::Mat difference, unitaryDifference;
		cv::absdiff(mat, previousFrame, difference);

		//Convert it to monochannel so we can count zeroes
		if (difference.type() == CV_8UC3) {
			cv::cvtColor(difference, difference, cv::COLOR_BGR2GRAY,1);
		}

		//Get the number of different pixels in the difference
		int differentCount = cv::countNonZero(difference);

		//If the relative amount of pixels is over a certain threshold, the frames are different
		if (((double)differentCount)/(difference.rows* difference.cols) > 0.1) {
			return false;
		}

		return true;
	}
}

