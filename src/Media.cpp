//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/Media.hpp"
#include "Video.hpp"
#include "Image.hpp"
#include "fonttik/Log.h"
#include "fonttik/ConfigurationParams.hpp"

namespace tik
{

class Frame;

Media* Media::createMedia(std::string mediaSource, ColorblindFilters* colorblindFilters)
{
	Media* media = nullptr;

	//Attempt to open file as image, if not possible then try as video
	cv::Mat img = cv::imread(mediaSource, cv::IMREAD_COLOR);
	
	if (!img.empty()) 
	{
		media = new Image(mediaSource, img, colorblindFilters);
	}
	else 
	{
		try
		{
			media = new Video(mediaSource);
		}
		catch (...)
		{
			LOG_CORE_DEBUG("Media {} can't be created as image or video", mediaSource);
		}
	}

	return media;
}

void Media::calculateMask(const MaskParams& maskParams)
{
	//only set mask when parameters have been defined
	if (!maskParams.ignoreMasks.empty() || !maskParams.focusMasks.empty() && maskParams.focusMasks[0] != cv::Rect2f{0, 0, 1, 1})
	{
		mask = cv::Mat(imageSize.height, imageSize.width, CV_8UC3, {0,0,0});

		//Regions inside focus masks are not ignored
		for (cv::Rect2f maskRect : maskParams.focusMasks) 
		{
			cv::Rect rectInImg(maskRect.x * imageSize.width, maskRect.y * imageSize.height,
				maskRect.width * imageSize.width, maskRect.height * imageSize.height);

			cv::Mat rectRegion = mask(rectInImg);
			rectRegion.setTo(cv::Scalar(255, 255, 255));
		}

		//Ignore masks will be ignored even if inside focus regions
		for (cv::Rect2f maskRect : maskParams.ignoreMasks) 
		{
			cv::Rect rectInImg(maskRect.x * imageSize.width, maskRect.y * imageSize.height,
				maskRect.width * imageSize.width, maskRect.height * imageSize.height);

			cv::Mat rectRegion = mask(rectInImg);
			rectRegion.setTo(cv::Scalar(0, 0, 0));
		}
	}
}

void Media::saveOutputData(cv::Mat data, fs::path path) 
{
	cv::imwrite(path.string(), data);
}

fs::path Media::getOutputPath() 
{
	fs::path path{ mediaSource };

	std::string out;

	//If input is a local file
	if (fs::exists(path))
	{
		out = path.parent_path().string() + "/" + (path.stem().string() + "_output");
	}
	else
	{
		out = { std::string{"./output/"} };
	}
	
	fs::path outputPath = fs::path{ out };
	if (!fs::is_directory(outputPath) || !fs::exists(outputPath)) 
	{
		fs::create_directory(outputPath);
	}

	return outputPath;
}

}//Namespace close bracket