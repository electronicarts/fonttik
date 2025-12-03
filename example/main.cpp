//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/Fonttik.hpp"
#include "fonttik/Configuration.hpp"
#include "fonttik/Media.hpp"
#include "fonttik/Log.h"

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

const std::regex outputDir("_output$"); //ignored directories

char* getCmdOption(char** begin, char** end, const std::string& option)
{
	char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

void processMedia(tik::Fonttik& fonttik, fs::path path, tik::Configuration& config, bool async) 
{
	tik::Media* media = tik::Media::createMedia(path.string(), fonttik.colorblindFilters);
	
	if (media != nullptr) 
	{
		if (async) 
		{
			fonttik.processMediaAsync(*media);
		}
		else 
		{
			tik::Results results = fonttik.processMedia(*media);
			fonttik.saveResults(*media, results);
			LOG_CORE_INFO("Storing json");
			fonttik.saveResultsToJson(media->getOutputPath(), results);
		}
		delete media;		
	}
	else
	{
		LOG_CORE_ERROR("{0} format is not supported", path.filename().string());
	}
}

//Recursively analyze all files in folder except for subfolders which correspond to outputs
void processFolder(tik::Fonttik& fonttik, fs::path path, tik::Configuration& config, bool async) {
	for (const auto& directoryEntry : fs::directory_iterator(path)) {
		if (fs::is_regular_file(directoryEntry)) {
			processMedia(fonttik, directoryEntry, config, async);
		}
		//Ignore output results
		else if (fs::is_directory(directoryEntry)) {
			//Avoid endless recursion produced by analysing results and producing more results to analyse
			if (std::regex_search(directoryEntry.path().string(), outputDir)) {
				LOG_CORE_TRACE("{0} is already a results folder", directoryEntry.path().string());
			}
			else {
				processFolder(fonttik, directoryEntry, config, async);
			}
		}
	}
}


int main(int argc, char* argv[]) {
	tik::Log::InitCoreLogger(true, false, 1, nullptr, "%^[%l] %v%$");
	LOG_CORE_WARNING("Note: The results shown in this report are for informational purposes only, and should not be used as a certification or validation of compliance with any legal, regulatory or other requirements.");
	
	LOG_CORE_TRACE("Executing in {0}", std::filesystem::current_path().string());

	std::cout << cv::getBuildInformation() << std::endl;

	bool async = cmdOptionExists(argv, argv + argc, "-a");

	fs::path path;
	if (argc < 2) 
	{
		LOG_CORE_INFO("Usage: \"{} media_path/", argv[0]); 
		LOG_CORE_INFO("Please, add the path of the file or directory you want to analyse");
		
		std::string p;
		std::cin >> p;
		
		try 
		{
			path = fs::path(p);
		}
		catch (...) 
		{
			LOG_CORE_ERROR("{0} is not a valid path", p);
			std::cin.get();
			return 1;
		}
	}
	else {
		path = fs::path(argv[1]);
	}


	tik::Fonttik fonttik = tik::Fonttik();
	char* configFilename = getCmdOption(argv, argv + argc, "-c");
	const char* configPath((configFilename) ? configFilename : "config.json");
	
	if (!fs::exists(configPath)) 
	{
		
		LOG_CORE_ERROR("{0} configuration file was not found, using default file", configPath);
		configPath = "config.json";
	}

 	tik::Configuration config = tik::Configuration(configPath);

	char* analysisWaitSeconds = getCmdOption(argv, argv + argc, "-s");
	
	if (analysisWaitSeconds)
	{
		int aws = std::atoi(analysisWaitSeconds);
		config.setAnalysisWaitSeconds(aws);
	}
	
	fonttik.init(&config);

	if (fs::exists(path) || path.string().rfind("http",0)==0/*begins with http*/ ) {

		if (!fs::is_directory(path)) {
			processMedia(fonttik, path, config, async);
		}
		else {
			processFolder(fonttik, path, config, async);
		}
	}
	else {
		LOG_CORE_ERROR("Path \"{0}\" not found", path.string());
		LOG_CORE_ERROR("CWD \"{0}\" ",fs::current_path().string() );
	}
}