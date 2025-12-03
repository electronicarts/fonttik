//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "Video.hpp"
#include "fonttik/Log.h"
#include <nlohmann/json.hpp>
#include <optional>

namespace tik
{

int Video::framesToSkip = 0;
int Video::fps = 0;

Video::Video(std::string mediaSource) : Media(mediaSource), msTimeStamp{ 0 }
{
	video.open(mediaSource);

	if (video.isOpened())
	{
		frameIndex = -1;
		cv::Mat frame;
		do
		{
			video >> frame;
			frameIndex++;
		} while (frame.empty());

		//reset position to previous frame
		frameIndex--;
		video.set(cv::CAP_PROP_POS_FRAMES, frameIndex);

		LOG_CORE_INFO("Starting processing with video frame {}", frameIndex);

		imageSize = frame.size();

		fps = round(video.get(cv::CAP_PROP_FPS));

	}
	else
	{
		LOG_CORE_ERROR("Video {} could not be opened", mediaSource);
	}
}


bool Video::loadFrame()
{
	

	auto getLength = [](const cv::VideoCapture& capture) { return (capture.get(cv::CAP_PROP_POS_FRAMES) + 1.0f) / (double)capture.get(cv::CAP_PROP_FRAME_COUNT); };
	auto finishedVideo = [&getLength](const cv::VideoCapture& capture) {return  getLength(capture) >= 1; };

	if (!currentFrame.empty())
	{
		previousFrame = currentFrame.clone();

		for (int i = 1; i < framesToSkip && !currentFrame.empty(); i++)
		{
			//Skip the amount of frames specified by configuration
			video >> currentFrame;
			frameIndex++;
		}

		while ((!currentFrame.empty() && compareFramesSimilarity(previousFrame, currentFrame))
			|| (currentFrame.empty() && !finishedVideo(video))) 
		{
			//Keep loading new frames until video is over or we find one that is 
			//sufficiently different from the previously processed one
			video >> currentFrame;
			frameIndex++;
		}

		LOG_CORE_INFO("Processing video frame {0} - {1:.3}%", frameIndex, std::min(getLength(video) * 100, 100.0));

		msTimeStamp = 1000.0 * (double)frameIndex / fps;
		return !currentFrame.empty();
	}
	else
	{
		frameIndex++;
		return video.read(currentFrame);
	}
}

Frame Video::getFrame()
{
	return Frame(currentFrame.clone(), mask, frameIndex, msTimeStamp);
}

void storeResultsInFrame(cv::Mat frameCopy, const std::vector<ResultBox>& res, Media::SaveResultProperties props, bool decimals, cv::VideoWriter out, cv::Size size) 
{
	//Write boxes
	for (const ResultBox& box : res) 
	{
		cv::Scalar color = props.colors[box.type];
		Frame::paintTextBox(box.x, box.y, box.width, box.height, color, frameCopy);
	}

	//Add measurements after boxes so boxes don't cover the numbers
	if (props.saveNumbers) 
	{
		for (const ResultBox& box : res) 
		{
			Frame::paintTextBoxResultValues(frameCopy, box, box.value, decimals); //Only add decimals with contrast checks
		}
	}

	//Scale down to avoid large size videos
	cv::resize(frameCopy, frameCopy, size);

	//Store
	out << frameCopy;
};


std::pair<fs::path, fs::path> Video::saveResultsOutlines(const SaveResultProperties& sizeResultProperties, const SaveResultProperties& contrastResultProperties)
{

	LOG_CORE_DEBUG("Storing results at {}", getOutputPath().string());

	//Create output path
	fs::path outputPath = getOutputPath();

	//Reopen video to start from beginning
	auto inputVideo = cv::VideoCapture(getPath().string());

	//Get size from input video and cap it at 1080p
	cv::Size size = cv::Size((int)inputVideo.get(cv::CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT));
	if (size.width > 1920 || size.height > 1080)
	{
		size.width = 1920;
		size.height = 1080;
	}

	//Open videos for writting
	auto OutVideoSize = CreateOutputVideoWritter(sizeResultProperties.path, size, fps);
	auto OutVideoContrast = CreateOutputVideoWritter(contrastResultProperties.path, size, fps);

	//Skip invalid frames until the video correctly begins
	cv::Mat frameMat;
	do { inputVideo >> frameMat; } while (frameMat.empty());

	//Iterate through every video frame
	//If specific result is not available for that frame, reuse previous result
	int resultIndex = 0, frameIndex = 0;
	while (!frameMat.empty()) 
	{
		std::thread t1(storeResultsInFrame, frameMat.clone(), std::ref(sizeResultProperties.results[resultIndex].results), sizeResultProperties, false, OutVideoSize, size);
		std::thread t2(storeResultsInFrame, frameMat.clone(), std::ref(contrastResultProperties.results[resultIndex].results), contrastResultProperties, true, OutVideoContrast, size);
		
		//get next frame
		frameIndex++;
		inputVideo >> frameMat;
		if (!frameMat.empty() && !mask.empty())
		{
			frameMat = frameMat & mask;
		}

		//Wait to be done writting
		t1.join();
		t2.join();

		// if new frame loaded corresponds to next available result then get next result index
		if (resultIndex + 1 < sizeResultProperties.results.size() && frameIndex == sizeResultProperties.results[resultIndex + 1].frame) 
		{
			resultIndex++;
		}
	}

	//Return resulting videos
	return { sizeResultProperties.path, contrastResultProperties.path };
}

void Video::saveResultsOutlinesAsync(const SaveResultProperties& sizeResultProperties, const SaveResultProperties& contrastResultProperties,
	rigtorp::SPSCQueue<FrameResult>& queue, std::atomic<bool>& done)
{
	LOG_CORE_DEBUG("Storing results at {}", getOutputPath().string());
	//Create output path
	fs::path outputPath = getOutputPath();

	//Reopen video to start from beginning
	auto inputVideo = cv::VideoCapture(getPath().string());

	//Get size from input video and cap it at 1080p
	cv::Size size = cv::Size((int)inputVideo.get(cv::CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT));
	if (size.width > 1920 || size.height > 1080)
	{
		size.width = 1920;
		size.height = 1080;
	}

	//Open videos for writting
	auto OutVideoSize = CreateOutputVideoWritter(sizeResultProperties.path, size, inputVideo.get(cv::CAP_PROP_FPS));
	auto OutVideoContrast = CreateOutputVideoWritter(contrastResultProperties.path, size, inputVideo.get(cv::CAP_PROP_FPS));

	std::filesystem::path outputSize = outputPath / "sizeChecks.json";
	std::filesystem::path outputContrast = outputPath / "contrastChecks.json";
	std::ofstream outSizeJson(outputSize);
	std::ofstream outContrastJson(outputContrast);
	outSizeJson << "[\n";
	outContrastJson << "[\n";
	//Skip invalid frames until the video correctly begins
	cv::Mat frameMat;
	do { inputVideo >> frameMat; } while (frameMat.empty());

	//Iterate through every video frame
	//If specific result is not available for that frame, reuse previous result
	int resultIndex = 0, frameIndex = 0;
	std::optional<FrameResult> previous{ std::nullopt }; //starts empty
	while (!(queue.empty() && done))
	{
		if (queue.front()) 
		{
			FrameResult current = *queue.front();
			LOG_CORE_TRACE("Storing frame {}", current.frameID);
			queue.pop();

			while (frameIndex < current.size.frame) 
			{
				if (previous) 
				{
					if (!frameMat.empty() && !mask.empty())
					{
						frameMat = frameMat & mask;
					}

					std::thread t1(storeResultsInFrame, frameMat.clone(), std::ref(previous.value().size.results), std::ref(sizeResultProperties), false, OutVideoSize, size);
					std::thread t2(storeResultsInFrame, frameMat.clone(), std::ref(previous.value().contrast.results), std::ref(contrastResultProperties), true, OutVideoContrast, size);
					storeResultsInJSON(previous.value().size.results, frameIndex, previous.value().timeStamp, outSizeJson);
					storeResultsInJSON(previous.value().contrast.results, frameIndex, previous.value().timeStamp, outContrastJson);

					//get next frame
					frameIndex++;
					inputVideo >> frameMat;


					//Wait to be done writting
					t1.join();
					t2.join();
				}
				else {
					//get next frame
					frameIndex++;
					inputVideo >> frameMat;
				}
			}
			previous = current;
		}
	}

	int nFrames = (int)inputVideo.get(cv::CAP_PROP_FRAME_COUNT);

	if (done && frameIndex != nFrames)
	{
		LOG_CORE_DEBUG("Not all frames stored");
		LOG_CORE_DEBUG("Pending frames: {}", nFrames - frameIndex);

		if (!previous)
		{
			throw std::logic_error("Previous frame is empty, cannot store results");
		}

		while (frameIndex < nFrames && !frameMat.empty()) //store remaining frames
		{
			LOG_CORE_DEBUG("Store frame: {}", frameIndex);
			if (!frameMat.empty() && !mask.empty())
			{
				frameMat = frameMat & mask;
			}

			std::thread t1(storeResultsInFrame, frameMat.clone(), std::ref(previous.value().size.results), std::ref(sizeResultProperties), false, OutVideoSize, size);
			std::thread t2(storeResultsInFrame, frameMat.clone(), std::ref(previous.value().contrast.results), std::ref(contrastResultProperties), true, OutVideoContrast, size);
			storeResultsInJSON(previous.value().size.results, frameIndex, previous.value().timeStamp, outSizeJson);
			storeResultsInJSON(previous.value().contrast.results, frameIndex, previous.value().timeStamp, outContrastJson);

			frameIndex++;
			inputVideo >> frameMat;

			//Wait to be done writting
			t1.join();
			t2.join();
		}

		LOG_CORE_TRACE("Pending frames stored");
	}

	outSizeJson.seekp((long)outSizeJson.tellp() - 2l);
	outSizeJson << "]\n";
	outContrastJson.seekp((long)outContrastJson.tellp() - 2l);
	outContrastJson << "]\n";
}


void Video::storeResultsInJSON(const std::vector<ResultBox>& res, int id, const std::string& timeStamp, std::ofstream& out)
{
	using json = nlohmann::json;
	json jFrame = json();
	jFrame["id"] = id;
	jFrame["timeStamp"] = timeStamp;
	jFrame["results"] = json::array();

	for (auto res : res)
	{
		json jResult = json();
		jResult["type"] = tik::ResultTypeAsString(res.type);
		jResult["x"] = res.x;
		jResult["y"] = res.y;
		jResult["width"] = res.width;
		jResult["height"] = res.height;
		jResult["value"] = res.value;
		jResult["text"] = res.text;

		jFrame["results"].push_back(jResult);
	}
	out << jFrame.dump();
	out << ",\n";

}

cv::VideoWriter Video::CreateOutputVideoWritter(const fs::path& outputPath, cv::Size outputSize, double FPS)
{
	cv::VideoWriter outputVideo(outputPath.string(), cv::VideoWriter::fourcc('a', 'v', 'c', '1'), FPS, outputSize, true);

	if (!outputVideo.isOpened())
	{
		LOG_CORE_CRITICAL("Unable to open {} for output video results", outputPath.string());
		throw std::runtime_error("Unable to store results");
	}
	return outputVideo;
}

bool Video::compareFramesSimilarity(const cv::Mat& mat1, const cv::Mat& mat2) 
{
	cv::Mat difference;
	cv::absdiff(mat1, mat2, difference);

	//Convert it to monochannel so we can count zeroes
	if (difference.type() == CV_8UC3) 
	{
		cv::cvtColor(difference, difference, cv::COLOR_BGR2GRAY, 1);
	}

	//Get the number of different pixels in the difference
	int differenceCount = cv::countNonZero(difference);

	//If the relative amount of pixels is over a certain threshold, the frames are different
	if (((double)differenceCount) / (difference.rows * difference.cols) > 0.1) 
	{
		return false;
	}

	return true;
}

void Video::setAnalysisWaitSeconds(int aws)
{
	framesToSkip = fps * aws;
	LOG_CORE_INFO("Skipping every {} seconds, every {} frames", aws, framesToSkip);
}

}