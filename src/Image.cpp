//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "Image.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace tik
{

Image::Image(std::string mediaSource, cv::Mat img) : Media(mediaSource), frame(img, mask, 0), processed(false)
{
	imageSize = img.size();
}


bool Image::loadFrame()
{
	return !processed;
}

Frame Image::getFrame()
{
	processed = true;
	return frame;
}

std::pair<fs::path, fs::path>Image::saveResultsOutlines(const SaveResultProperties& sizeResultProperties,
	const SaveResultProperties& contrastResultProperties) 
{
	return 
	{
		saveResultsOutlines(sizeResultProperties.results, sizeResultProperties.path, sizeResultProperties.colors, sizeResultProperties.saveNumbers),
		saveResultsOutlines(contrastResultProperties.results, contrastResultProperties.path, contrastResultProperties.colors, contrastResultProperties.saveNumbers),
	};
}

void Image::calculateMask(const MaskParams& params){
	Media::calculateMask(params);
	frame={ frame.getFrameMat(), mask, 0 };
}

void Image::saveResultsOutlinesAsync(const SaveResultProperties& sizeResultProperties, const SaveResultProperties& contrastResultProperties, 
	rigtorp::SPSCQueue<FrameResult>& queue, std::atomic<bool>& done)
{
	SaveResultProperties sizeProps = sizeResultProperties;
	SaveResultProperties contrastProps = contrastResultProperties;
	fs::path outputPath = getOutputPath();
	std::filesystem::path outputSize = outputPath / "sizeChecks.json";
	std::filesystem::path outputContrast = outputPath / "contrastChecks.json";
	std::ofstream outSizeJson(outputSize);
	std::ofstream outContrastJson(outputContrast);

	outSizeJson << "[\n";
	outContrastJson << "[\n";
	while (!(queue.empty() && done))
	{
		if (queue.front()) {
			FrameResult current = *queue.front();
			queue.pop();
			sizeProps.results.push_back(current.size);
			contrastProps.results.push_back(current.contrast);
			saveResultsOutlines(sizeProps, contrastProps);
			storeResultsInJSON(current.size.results, 0, outSizeJson);
			storeResultsInJSON(current.contrast.results, 0, outContrastJson);

		}
	}
	outSizeJson.seekp((long)outSizeJson.tellp() - 2l);
	outSizeJson << "]\n";
	outContrastJson.seekp((long)outContrastJson.tellp() - 2l);
	outContrastJson << "]\n";
}

void Image::storeResultsInJSON(const std::vector<ResultBox>& res, int id, std::ofstream& out) 
{
	using json = nlohmann::json;
	json jFrame = json();
	jFrame["id"] = id;

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



fs::path Image::saveResultsOutlines(const std::vector<FrameResults>& results, fs::path path, 
	const std::vector<cv::Scalar>& colors, bool saveNumbers) 
{
	cv::Mat highlights = frame.getFrameMat().clone();

	for (const ResultBox& box : results.back().results) 
	{
		cv::Scalar color = colors[box.type];
		Frame::paintTextBox(box.x, box.y, box.x + box.width, box.y + box.height, color, highlights, 2);
	}

	//Add measurements after outline so it doesn't cover the numbers
	if (saveNumbers) 
	{
		for (const ResultBox& box : results.back().results) 
		{
			Frame::paintTextBoxResultValues(highlights, box, (path.stem() == "contrastChecks") ? 1 : 0); //Only add decimals with contrast checks
		}
	}

	saveOutputData(highlights, path.string());
	return path;
}

}