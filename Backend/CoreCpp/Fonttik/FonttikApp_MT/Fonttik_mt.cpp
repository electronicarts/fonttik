//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "FrameProcessor.h"
#include "Configuration.h"
#include "Media.h"
#include "Log.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <thread>

namespace fs = std::filesystem;

const std::regex outputDir("_output$");

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

char* getCmdOption(char** begin, char** end, const std::string& option)
{
	char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

void processMedia(const std::vector<tik::FrameProcessor*>& workers, fs::path path, tik::Configuration& config) {
	tik::Media* media = tik::Media::CreateMedia(path);

	if (media != nullptr) {
		//TODO This implementation only improves video performance, if different images need to be processed
		//in parallel, different media processor threads would need to be created
		std::vector<std::thread> threads;
		std::mutex media_mtx;
		for (auto& worker : workers) {
			threads.push_back(std::thread(&tik::FrameProcessor::work, worker, media, &media_mtx));
		}

		for (auto& thread : threads) {
			thread.join();
		}

		tik::Results* results = media->getResultsPointer();
		LOG_CORE_INFO("SIZE CHECK RESULT: {0}", (results->sizePass() ? "PASS" : "FAIL"));
		LOG_CORE_INFO("CONTRAST CHECK RESULT: {0}", (results->contrastPass() ? "PASS" : "FAIL"));

		//if specified in config, save outlines for both size and contrast
		tik::AppSettings* appSettings = config.getAppSettings();
		if (appSettings->saveTexboxOutline()) {
			media->saveResultsOutlines(results->getContrastResults(),
				media->getOutputPath() / "contrastChecks", appSettings->getColors(),
				appSettings->printValuesOnResults());
			media->saveResultsOutlines(results->getSizeResults(),
				media->getOutputPath() / "sizeChecks", appSettings->getColors(),
				appSettings->printValuesOnResults());
		}

		delete media;
	}
	else
	{
		LOG_CORE_ERROR("{0} format is not supported", path.filename().string());
	}
}

void processFolder(const std::vector<tik::FrameProcessor*>& workers, fs::path path, tik::Configuration& config) {
	for (const auto& directoryEntry : fs::directory_iterator(path)) {
		if (fs::is_regular_file(directoryEntry)) {
			processMedia(workers, directoryEntry, config);
		}
		//Ignore output results
		else if (fs::is_directory(directoryEntry)) {
			//Avoid endless recursion produced by analysing results and producing more results to analyse
			if (std::regex_search(directoryEntry.path().string(), outputDir)) {
				LOG_CORE_TRACE("{0} is already a results folder", directoryEntry.path().string());
			}
			else {
				processFolder(workers, directoryEntry, config);
			}
		}
	}
}

int main(int argc, char* argv[]) {
	tik::Log::InitCoreLogger(true, false, 0, nullptr, "%^[T] [%l] %v%$");

	LOG_CORE_TRACE("Executing in {0}", std::filesystem::current_path().string());

	fs::path path;


	if (argc < 2) {
		std::cout << "Usage: \""<<argv[0]<< "media_path/ \n" <<
			"Please, add the path of the file or directory you want to analyse \n";
		std::string p;
		std::cin >> p;
		try {
			path = fs::path(p);
		}
		catch (...) {
			LOG_CORE_ERROR("{0} is not a valid path", p);
			std::cin.get();
			return 1;
		}
	}
	else {
		path = fs::path(argv[1]);
	}

	char* configFilename = getCmdOption(argv, argv + argc, "-c");

	std::string configPath((configFilename) ? configFilename : "config.json");
	if (!fs::exists(configPath)) {
		LOG_CORE_ERROR("{0} configuration file was not found, using default file", configPath);
		configPath = std::string("config.json");
	}
	tik::Configuration config = tik::Configuration(configPath);

	char* opt_threads = getCmdOption(argv, argv + argc, "-t");
	int n_threads;
	//Number of threads specified by argument, minimum is 1
	try {
		n_threads = (opt_threads) ? std::stoi(opt_threads) : 1;
	}
	catch (std::exception const& e)
	{
		LOG_CORE_ERROR("Invalid thread number");
		return 1;
	}
	int max_threads = std::thread::hardware_concurrency();
	if (n_threads > max_threads) {
		LOG_CORE_WARNING("Too many threads, using system max of {0}", max_threads);
		n_threads = max_threads;
	}
	
	std::vector<tik::FrameProcessor*> workers;

	//Allocation of frame processors
	for (int i = 0; i < n_threads; i++) {
		workers.emplace_back(new tik::FrameProcessor());
	}
	//Initialisation of frame processors
	for (auto& worker : workers) {
		worker->init(&config);
	}


	if (fs::exists(path)) {

		if (!fs::is_directory(path)) {
			processMedia(workers, path, config);
		}
		else {
			processFolder(workers, path, config);
		}
	}
	else {
		LOG_CORE_ERROR("Path \"{0}\" not found", path.string());
	}

	for (auto it = workers.begin(); it != workers.end(); ++it) {
		delete* it;
	}

}