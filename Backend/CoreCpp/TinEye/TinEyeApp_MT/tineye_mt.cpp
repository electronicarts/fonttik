#include "FrameProcessor.h"
#include "Configuration.h"
#include "Media.h"
#include "Instrumentor.h"

#include <iostream>
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
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

void processMedia(const std::vector<tin::FrameProcessor*>& workers, fs::path path, tin::Configuration& config) {
	tin::Media* media = tin::Media::CreateMedia(path);

	if (media != nullptr) {
		//TODO This implementation only improves video performance, if different images need to be processed
		//If further improvements were needed for single images, media processor threads would need to be created
		std::vector<std::thread> threads;
		std::mutex media_mtx;
		for (auto& worker : workers) {
			threads.push_back(std::thread(&tin::FrameProcessor::work, worker, media, &media_mtx));
		}

		for (auto& thread : threads) {
			thread.join();
		}

		tin::Results* results = media->getResultsPointer();
		std::cout << "SIZE: " << ((results->sizePass()) ? "PASS" : "FAIL") <<
			"\tCONTRAST: " << ((results->contrastPass()) ? "PASS" : "FAIL") << std::endl;

		//if specified in config, save outlines for both size and contrast
		tin::AppSettings* appSettings = config.getAppSettings();
		if (appSettings->saveTexboxOutline()) {
			media->saveResultsOutlines(results->getContrastResults(),
				media->getOutputPath() / "contrastChecks",
				appSettings->printValuesOnResults());
			media->saveResultsOutlines(results->getSizeResults(),
				media->getOutputPath() / "sizeChecks",
				appSettings->printValuesOnResults());
		}

		delete media;
	}
	else
	{
		std::cerr << path.filename() << " format is not supported" << std::endl;
	}
}

void processFolder(const std::vector<tin::FrameProcessor*>& workers, fs::path path, tin::Configuration& config) {
	for (const auto& directoryEntry : fs::directory_iterator(path)) {
		if (fs::is_regular_file(directoryEntry)) {
			processMedia(workers, directoryEntry, config);
		}
		//Ignore output results
		else if (fs::is_directory(directoryEntry)) {
			//Avoid endless recursion produced by analysing results and producing more results to analyse
			if (std::regex_search(directoryEntry.path().string(), outputDir)) {
				std::cout << directoryEntry << "is already a results folder" << std::endl;
			}
			else {
				processFolder(workers, directoryEntry, config);
			}
		}
	}
}

int main(int argc, char* argv[]) {
	Instrumentor::Get().BeginSession("Profile", "profiling.json");

	BOOST_LOG_TRIVIAL(trace) << "Executing in " << std::filesystem::current_path() << std::endl;
	boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%Severity%] %Message%");

	fs::path path;


	if (argc < 2) {
		std::cout << "Usage: \"TinEyeApp.exe media_path/ \n" <<
			"Please, add the path of the file or directory you want to analyse \n";
		std::string p;
		std::cin >> p;
		try {
			path = fs::path(p);
		}
		catch (...) {
			BOOST_LOG_TRIVIAL(error) << p << " is not a valid path";
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
		BOOST_LOG_TRIVIAL(error) << configPath << " configuration file was not found, using default file";
		configPath = std::string("config.json");
	}
	tin::Configuration config = tin::Configuration(configPath);

	int n_threads = std::thread::hardware_concurrency();
	std::vector<tin::FrameProcessor*> workers;

	for (int i = 0; i < n_threads; i++) {
		workers.emplace_back(new tin::FrameProcessor());
		workers.back()->init(&config);
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
		std::cerr << "Path not found " << std::endl;
	}

	for (auto it = workers.begin(); it != workers.end(); ++it) {
		delete* it;
	}

	Instrumentor::Get().EndSession();
}