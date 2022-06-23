#include "TinEye.h"
#include "Configuration.h"
#include "Media.h"
#include "Instrumentor.h"

#include <iostream>
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
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

void processMedia(tin::TinEye& tineye, fs::path path, tin::Configuration& config) {
	tin::Media* media = tin::Media::CreateMedia(path);

	if (media != nullptr) {
		tin::Results* results = tineye.processMedia(*media);

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

void processFolder(tin::TinEye& tineye, fs::path path, tin::Configuration& config) {
	for (const auto& directoryEntry : fs::directory_iterator(path)) {
		if (fs::is_regular_file(directoryEntry)) {
			processMedia(tineye, directoryEntry, config);
		}
		//Ignore output results
		else if (fs::is_directory(directoryEntry)) {
			//Avoid endless recursion produced by analysing results and producing more results to analyse
			if (std::regex_search(directoryEntry.path().string(), outputDir)) {
				std::cout << directoryEntry << "is already a results folder" << std::endl;
			}
			else {
				processFolder(tineye, directoryEntry, config);
			}
		}
	}
}

int main(int argc, char* argv[]) {
	
	BOOST_LOG_TRIVIAL(trace) << "Executing in " << std::filesystem::current_path() << std::endl;
	boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%Severity%] %Message%");
	Instrumentor::Get().BeginSession("Profile", "profiling.json");

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


	tin::TinEye tineye = tin::TinEye();
	char* configFilename = getCmdOption(argv, argv + argc, "-c");
	std::string configPath((configFilename) ? configFilename : "config.json");
	if (!fs::exists(configPath)) {
		BOOST_LOG_TRIVIAL(error) << configPath << " configuration file was not found, using default file";
		configPath = std::string("config.json");
	}
 	tin::Configuration config = tin::Configuration(configPath);
	tineye.init(&config);

	if (fs::exists(path)) {

		if (!fs::is_directory(path)) {
			processMedia(tineye, path, config);
		}
		else {
			processFolder(tineye, path, config);
		}
	}
	else {
		std::cerr << "Path not found " << std::endl;
	}

	Instrumentor::Get().EndSession();
}