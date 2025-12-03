//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "Image.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace tik
{

Image::Image(std::string mediaSource, cv::Mat img, ColorblindFilters* colorblindFilters) : Media(mediaSource), frame(img, mask, 0), processed(false),
protanFrame(img, mask, 0), deutanFrame(img, mask, 0), tritanFrame(img, mask, 0), grayscaleFrame(img, mask, 0)
{
	imageSize = img.size();
	
	if (colorblindFilters != nullptr)
	{
		std::vector<cv::Mat> colorblindImgs = colorblindFilters->applyColorblindFilters(img);
		protanFrame = Frame(colorblindImgs[0], mask);
		deutanFrame = Frame(colorblindImgs[1], mask);
		tritanFrame = Frame(colorblindImgs[2], mask);
		grayscaleFrame = Frame(colorblindImgs[3], mask);
	}	
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

std::vector<Frame>Image::getColorblindFrames()
{
	return { protanFrame, deutanFrame, tritanFrame, grayscaleFrame };
}

void Image::saveColorblindImages() {
	fs::path path = getOutputPath();
	std::vector<fs::path> colorblindPaths = {
			path / fs::path{ std::string("protanImage") + getExtension() },
			path / fs::path{ std::string("deutanImage") + getExtension() },
			path / fs::path{ std::string("tritanImage") + getExtension() },
			path / fs::path{ std::string("grayscaleImage") + getExtension() }
	};

	std::vector<cv::Mat> colorblindFrames = { protanFrame.getFrameMat().clone(), deutanFrame.getFrameMat().clone(),
		tritanFrame.getFrameMat().clone(), grayscaleFrame.getFrameMat().clone() };

	for (int i = 0; i < 4; i++) {
		saveOutputData(colorblindFrames[i], colorblindPaths[i].string());
	}
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
			saveColorblindImages();
			saveResultsOutlines(sizeProps, contrastProps);
			storeResultsInJSON(current.size.results, 0, outSizeJson);
			storeResultsInJSON(current.contrast.results, 0, outContrastJson, true);
		}
	}
	outSizeJson.seekp((long)outSizeJson.tellp() - 2l);
	outSizeJson << "]\n";
	outContrastJson.seekp((long)outContrastJson.tellp() - 2l);
	outContrastJson << "]\n";
}

void Image::storeResultsInJSON(const std::vector<ResultBox>& res, int id, std::ofstream& out, bool contrast) 
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

			if (contrast)
			{
				jResult["protanValue"] = res.colorblindValues[0];
				jResult["protanType"] = tik::ResultTypeAsString(res.colorblindTypes[0]);
				jResult["deutanValue"] = res.colorblindValues[1];
				jResult["deutanType"] = tik::ResultTypeAsString(res.colorblindTypes[1]);
				jResult["tritanValue"] = res.colorblindValues[2];
				jResult["tritanType"] = tik::ResultTypeAsString(res.colorblindTypes[2]);
				jResult["grayscaleValue"] = res.colorblindValues[3];
				jResult["grayscaleType"] = tik::ResultTypeAsString(res.colorblindTypes[3]);
			}

			jFrame["results"].push_back(jResult);
		}
		out << jFrame.dump();
		out << ",\n";
}

fs::path Image::saveResultsOutlines(const std::vector<FrameResults>& results, fs::path path, 
	const std::vector<cv::Scalar>& colors, bool saveNumbers) 
{
	if (path.stem() == "contrastChecks")
	{
		std::vector<fs::path> colorblindPaths = {
			path.parent_path() / "protanChecks.png",
			path.parent_path() / "deutanChecks.png",
			path.parent_path() / "tritanChecks.png",
			path.parent_path() / "grayscaleChecks.png"
		};

		std::vector<cv::Mat> colorblindFrames = { protanFrame.getFrameMat().clone(), deutanFrame.getFrameMat().clone(),
			tritanFrame.getFrameMat().clone(), grayscaleFrame.getFrameMat().clone() };

		for (int i = 0; i < 4; i++)
		{
			cv::Mat frame = colorblindFrames[i];
			fs::path pathColorblind = colorblindPaths[i];
			
			for (const ResultBox& box : results.back().results) 
			{
				std::vector<double> colorblindValues = box.colorblindValues;
				cv::Scalar color = colors[box.type];
				Frame::paintTextBox(box.x, box.y, box.width, box.height, color, frame, 2);

				if (saveNumbers)
				{
					Frame::paintTextBoxResultValues(frame, box, colorblindValues[i], 1);
				}
			}
			saveOutputData(frame, pathColorblind.string());
		}
	}

	cv::Mat highlights = frame.getFrameMat().clone();

	for (const ResultBox& box : results.back().results) 
	{
		cv::Scalar color = colors[box.type];
		Frame::paintTextBox(box.x, box.y, box.width, box.height, color, highlights, 2);
	}

	//Add measurements after outline so it doesn't cover the numbers
	if (saveNumbers)
	{
		for (const ResultBox& box : results.back().results)
		{
			Frame::paintTextBoxResultValues(highlights, box, box.value, (path.stem() == "contrastChecks") ? 1 : 0); //Only add decimals with contrast checks
		}
	}

	saveOutputData(highlights, path.string());
	return path;
}

}