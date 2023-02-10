//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "Video.h"
#include "fonttik/Log.h"
#include "fonttik/Frame.h"

namespace tik {
	int Video::framesToSkip = 0;
	int Video::frameOutputInterval = 0;

	Video::Video(fs::path filePath, cv::VideoCapture capture) : Media(filePath), videoCapture(capture)
	{
		if (videoCapture.isOpened())
		{
			frameCount = -1;
			do {
				videoCapture >> imageMatrix;
				previousFrame = imageMatrix;
				frameCount++;
			} while (imageMatrix.empty());
			LOG_CORE_INFO("Starting processing with video frame {}, skipping every {} frames", frameCount, framesToSkip);
		}
		else {
			LOG_CORE_ERROR("Video {} could not be opened", fmt::ptr(filePath.c_str()));
		}
	}

	Video::~Video()
	{
		if (videoCapture.isOpened()) 
		{
			videoCapture.release();
		}
	}

	Frame* Video::getFrame() {

		Frame* frame = (imageMatrix.empty()) ? nullptr : new Frame(this, getFrameCount(), imageMatrix.clone());

		return frame;
	}

	bool Video::nextFrame()
	{
		bool ret = false;

		imageMatrix.release();
		auto getLength = [](const cv::VideoCapture& capture) { return (capture.get(cv::CAP_PROP_POS_FRAMES) + 1.0f) / (double)capture.get(cv::CAP_PROP_FRAME_COUNT); };
		auto finishedVideo = [&getLength](const cv::VideoCapture& capture) {return  getLength(capture) >= 1; };

		if (videoCapture.isOpened()) {
			videoCapture >> imageMatrix;

			for (int i = 0; i < framesToSkip && !imageMatrix.empty(); i++) {
				//Skip the amount of frames specified by configuration
				videoCapture >> imageMatrix;
				frameCount++;
			}

			while ((!imageMatrix.empty() && compareFramesSimilarity(previousFrame, imageMatrix))
				|| (imageMatrix.empty() && !finishedVideo(videoCapture))) {
				//Keep loading new frames until video is over or we find one that is 
				//sufficiently different from the previously processed one
				videoCapture >> imageMatrix;
				frameCount++;
			}

			LOG_CORE_INFO("Processing video frame {0} - {1:.3}%", ++frameCount, std::min(getLength(videoCapture) * 100, 100.0));

			previousFrame = imageMatrix;
			ret = !imageMatrix.empty();
		}
		else {
			ret = false;
		}

		return ret;
	}

	void Video::saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, const std::vector<cv::Scalar>& colors, bool saveNumbers) {
		//Create output path
		fs::path outputPath = getOutputPath();

		//Reopen video to start from beginning
		videoCapture.open(getPath().string());

		//Get size from input video
		cv::Size size = cv::Size((int)videoCapture.get(cv::CAP_PROP_FRAME_WIDTH), (int)videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));

		//Create output video
		cv::VideoWriter outputVideo(path.string() + ".mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
			videoCapture.get(cv::CAP_PROP_FPS), size, true);

		//Iterate through every video frame
		//If specific result is not available for that frame, reuse previous result
		int resultIndex = 0, frameIndex = 0, lastFrameGenerated = 0;
		cv::Mat frameMat, frameCopy;

		//Skip invalid frames until the video correctly begins
		do {
			videoCapture >> frameMat;
		} while (frameMat.empty());

		while (!frameMat.empty()) {
			//clone frame for editing
			frameCopy = frameMat.clone();

			for (ResultBox& box : results[resultIndex].results) {
				cv::Scalar color = colors[box.type];
				Frame::highlightBox(box.x, box.y, box.x + box.width, box.y + box.height, color, frameCopy, 2);
			}

			//Add measurements after boxes so boxes don't cover the numbers
			if (saveNumbers) {
				for (ResultBox& box : results[resultIndex].results) {
					Frame::putResultBoxValues(frameCopy, box, (path.stem() == "contrastChecks") ? 1 : 0); //Only add decimals with contrast checks
				}
			}

			//output frame to result video
			outputVideo << frameCopy;

			//Output frameCopy into an image if sufficient frames have gone by and frameOutputInterval is set and current frame has a detected fail
			if (frameOutputInterval != 0 && frameIndex - lastFrameGenerated > frameOutputInterval && !results[resultIndex].overallPass) {
				lastFrameGenerated = frameIndex;
				std::string pathSuffix = "_" + std::to_string(lastFrameGenerated) + ".png";
				saveOutputData(frameCopy, path.string() + pathSuffix);
			}

			//get next frame
			frameIndex++;
			videoCapture >> frameMat;

			// if new frame loaded corresponds to next available result then get next result index
			if (resultIndex + 1 < results.size() && frameIndex == results[resultIndex + 1].frame) {
				resultIndex++;
			}
		}
	}

	bool Video::compareFramesSimilarity(cv::Mat& mat1, cv::Mat& mat2) {
		cv::Mat difference, unitaryDifference;
		cv::absdiff(mat1, mat2, difference);

		//Convert it to monochannel so we can count zeroes
		if (difference.type() == CV_8UC3) {
			cv::cvtColor(difference, difference, cv::COLOR_BGR2GRAY, 1);
		}

		//Get the number of different pixels in the difference
		int differentCount = cv::countNonZero(difference);

		//If the relative amount of pixels is over a certain threshold, the frames are different
		if (((double)differentCount) / (difference.rows * difference.cols) > 0.1) {
			return false;
		}

		return true;
	}

	void Video::setFramesToSkip(int numberOfFrames)
	{
		framesToSkip = numberOfFrames;
	}

	void Video::setFrameOutputInterval(int numberOfFrames)
	{
		frameOutputInterval = numberOfFrames;
	}
}
