//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/Fonttik.hpp"
#include "fonttik/Media.hpp"
#include "fonttik/Log.h"
#include "fonttik/Results.h"
#include "fonttik/Configuration.hpp"
#include "fonttik/Frame.hpp"
#include "OCRFactory.h"
#include "fonttik/ConfigurationParams.hpp"
#include "SizeChecker.hpp"
#include "ContrastChecker.hpp"
#include "TextBoxRecognitionOpenCV.hpp"


namespace tik
{

void Fonttik::init(Configuration* config)
{
	configuration = config;

	textBoxDetection = OCRFactory::CreateTextboxDetection(configuration->getTextDetectionBackend(), configuration->getTextDetectionParams(), config->getSbgrValues());
	
	if (configuration->getTextSizeParams().useTextRecognition)
	{
		textBoxRecognition = OCRFactory::CreateTextboxRecognition(configuration->getTextRecognitionParams());
	}

	//Create checkers
	contrastChecker = new ContrastChecker(config);
	sizeChecker = new SizeChecker(config, textBoxRecognition);
}

std::pair<fs::path, fs::path> Fonttik::saveResults(Media& media, Results& results)
{
	if (!configuration->getAppSettings().saveTextboxOutline)
	{
		return {};
	}
	return media.saveResultsOutlines(
		{ results.getSizeResults(), configuration->getOutlineColors(), media.getOutputPath() / fs::path{std::string("sizeChecks") + media.getExtension()}, 
			configuration->getAppSettings().printResultValues },
		{ results.getContrastResults(), configuration->getOutlineColors(), media.getOutputPath() / fs::path{std::string("contrastChecks") + media.getExtension()},
			configuration->getAppSettings().printResultValues }
	);
}

std::pair<fs::path, fs::path> Fonttik::saveResultsToJson(fs::path outputPath, Results& results)
{
	fs::path outputSize = outputPath / "sizeChecks.json";
	fs::path outputContrast = outputPath / "contrastChecks.json";

	auto resultsToJSON = [](const fs::path& path, const std::vector<tik::FrameResults>& results)
		{
			json jResults;
			jResults = json::array();
			for (auto frame : results)
			{
				json jFrame = json();
				jFrame["id"] = (frame.frame);
				jFrame["results"] = json::array();

				for (auto res : frame.results)
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

				jResults.push_back(jFrame);
			}
			std::ofstream out(path);
			out << jResults.dump();
		};

	resultsToJSON(outputContrast, results.getContrastResults());
	resultsToJSON(outputSize, results.getSizeResults());

	return { outputSize, outputContrast };
}

AsyncResults tik::Fonttik::processMediaAsync(Media& media)
{
	if (!setResolutionGuideline(media))
	{
		throw std::runtime_error("Media resolution not supported");
	}

	Results results;
	media.calculateMask(configuration->getMaskParams());
	media.setAnalysisWaitSeconds(configuration->getAppSettings().analysisWaitSeconds);

	rigtorp::SPSCQueue<FrameResult> queue(10);
	std::atomic<bool> done = false;

	std::thread t([&]() {
		media.saveResultsOutlinesAsync(
			{ {}, configuration->getOutlineColors(), media.getOutputPath() / fs::path{std::string("sizeChecks") + media.getExtension()}, 
				configuration->getAppSettings().printResultValues },
			{ {}, configuration->getOutlineColors(), media.getOutputPath() / fs::path{std::string("contrastChecks") + media.getExtension()}, 
				configuration->getAppSettings().printResultValues },
			queue, done);
		});

	int count = 0;
	AsyncResults result;

	//Process each frame and add received frame's results to the media results
	while (media.loadFrame()) 
	{
		Frame frame = media.getFrame();
		std::pair<FrameResults, FrameResults> res = processFrame(frame,configuration->getAppSettings().sizeByLine);
		FrameResult frameResult{ res.first, res.second, count++, frame.getTimeStamp()};
		while (!queue.try_push(frameResult)) {}; //Busy wait for results to be consumed
		result.overAllPassSize = result.overAllPassSize && res.first.overallPass;
		result.overAllPassContrast = result.overAllPassContrast && res.second.overallPass;
	}
	done = true;
	t.join();
	result.pathToSizeResult = media.getOutputPath() / fs::path{ std::string("sizeChecks") + media.getExtension() };
	result.pathToContrastResult = media.getOutputPath() / fs::path{ std::string("contrastChecks") + media.getExtension() };
	result.pathToJSONSizeResult = media.getOutputPath() / fs::path{ std::string("sizeChecks.json") };
	result.pathToJSONContrastResult = media.getOutputPath() / fs::path{ std::string("contrastChecks.json") };

	return result;

}

Results Fonttik::processMedia(Media& media)
{
	
	/*	
	int activeSize = appSettings->getSpecifiedSize();
	guideline->setDPI(appSettings->usingDPI());
	guideline->setActiveGuideline((activeSize != 0) ? activeSize : nextFrame->getImageMatrix().rows);*/

	if (!setResolutionGuideline(media))
	{
		return {};
	}

	Results results;
	media.calculateMask(configuration->getMaskParams());
	media.setAnalysisWaitSeconds(configuration->getAppSettings().analysisWaitSeconds);

	while (media.loadFrame())
	{
		auto frame = media.getFrame();
		std::pair<FrameResults, FrameResults> res = processFrame(frame, configuration->getAppSettings().sizeByLine);
		results.addSizeResults(res.first);
		results.addContrastResults(res.second);
	}

	LOG_CORE_TRACE("SIZE CHECK RESULT: {0}", (results.sizePass() ? "PASS" : "FAIL"));
	LOG_CORE_TRACE("CONTRAST CHECK RESULT: {0}", (results.contrastPass() ? "PASS" : "FAIL"));
		
	return results;
}

std::pair<FrameResults, FrameResults> Fonttik::processFrame(Frame& frame, bool sizeByLine)
{
	//TODO:: Add condition on whether we are grouping by line or not for text size
	std::vector<tik::TextBox> words;
	std::vector<tik::TextBox> lines;

	if (sizeByLine)
	{
		auto textBoxes = textBoxDetection->detectLinesAndWords(frame.getFrameMat());
		words = textBoxes.words;
		lines = textBoxes.lines.empty()?textBoxes.words:textBoxes.lines;
	}
	else
	{
		words = textBoxDetection->detectBoxes(frame.getFrameMat());
		lines = words;
	}

	FrameResults sizeResults(-1);
	FrameResults contrastResults(-1);

	if (words.empty())
	{
		LOG_CORE_INFO("No words detected in image");
		return { sizeResults, contrastResults };
	}

	textBoxDetection->mergeTextBoxes(words, frame.getFrameMat());
	textBoxDetection->mergeTextBoxes(lines, frame.getFrameMat());
	
	calculateTextBoxLuminance(words);
	calculateTextBoxLuminance(lines);

	calculateTextMasks(words);
	calculateTextMasks(lines);

	contrastResults = contrastChecker->check(frame.getFrameIndex(), words);
	sizeResults = sizeChecker->check(frame.getFrameIndex(), lines);

	setTextInContrastResults(sizeResults,contrastResults);

	return { sizeResults, contrastResults };
}

bool Fonttik::setResolutionGuideline(const Media& media)
{
	if (configuration->getAppSettings().detectResolution)
	{
		LOG_CORE_INFO("Media resolution detection is enabled, using resolution: {}x{}",
			media.getImageSize().width, media.getImageSize().height);

		//Set width of image to match resolution
		// If within 100px of target resolution we accept it as good
		if (checkResolution(media.getImageSize(), RESOLUTION_2160p))
		{
			RESOLUTION_2160p.height;
			return configuration->setResolutionGuideline("2160");
		}
		else if (checkResolution(media.getImageSize(), RESOLUTION_1080p))
		{
			return configuration->setResolutionGuideline("1080");
		}
		else if (checkResolution(media.getImageSize(), RESOLUTION_720p))
		{
			return configuration->setResolutionGuideline("720");
		}
		else if (checkResolution(media.getImageSize(), RESOLUTION_STEAM_DECK))
		{
			return configuration->setResolutionGuideline("1080");
		}
		else
		{
			LOG_CORE_ERROR("Resolution not supported: {}x{}", media.getImageSize().width, media.getImageSize().height);
			return false;
		}
	}
	else
	{
		LOG_CORE_INFO("Resolution detection is disabled, using specified resolution: {}",
			configuration->getAppSettings().targetResolution);
		return configuration->setResolutionGuideline(configuration->getAppSettings().targetResolution);
	}
}

bool Fonttik::checkResolution(const cv::Size& mediaSize, const cv::Size& resolution)
{
	return (mediaSize.width + MAX_LEEWAY > resolution.width && mediaSize.width - MAX_LEEWAY <= resolution.width)
		&& (mediaSize.height + MAX_LEEWAY > resolution.height && mediaSize.height - MAX_LEEWAY <= resolution.height);
}

void Fonttik::calculateTextBoxLuminance(std::vector<TextBox>& textBoxes)
{
	for (auto& textBox : textBoxes)
	{
		textBox.calculateTextBoxLuminance(configuration->getSbgrValues());
	}
}

void Fonttik::calculateTextMasks(std::vector<TextBox>& textBoxes)
{
	for (auto& textBox : textBoxes)
	{
		textBox.calculateTextMask();
	}
}

void Fonttik::setTextInContrastResults(const FrameResults& sizeResults, FrameResults& contrastResults)
{
	for (int i = 0; i < contrastResults.results.size(); i++)
	{
		if (contrastResults.results[i].text == "" && sizeResults.results.size() > i)
		{
			contrastResults.results[i].text = sizeResults.results[i].text;
		}
	}
}

Fonttik::~Fonttik()
{
	if (textBoxDetection != nullptr) 
	{
		delete textBoxDetection;
	}

	if (textBoxRecognition != nullptr) 
	{
		delete textBoxRecognition;
	}

	if (contrastChecker != nullptr) 
	{
		delete contrastChecker;
	}

	if (sizeChecker != nullptr) 
	{
		delete sizeChecker;
	}

}

} //namescape tik