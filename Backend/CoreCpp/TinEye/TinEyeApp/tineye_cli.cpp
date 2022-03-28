#include "TinEye.h"
#include "Configuration.h"
#include <iostream>
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <filesystem>
#include <regex>
#include "Media.h"

namespace fs = std::filesystem;

const std::regex outputDir("_output$");

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

void processMedia(tin::TinEye& tineye, fs::path path, tin::Configuration& config) {
	tin::Media* media = tin::Media::CreateMedia(path);

	do {
		BOOST_LOG_TRIVIAL(debug) << "Using EAST preprocessing" << std::endl;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(*media);
		std::vector<tin::Textbox> textBoxes = tineye.getTextBoxes(*media);
		tineye.mergeTextBoxes(textBoxes);
		if (textBoxes.empty()) {
			BOOST_LOG_TRIVIAL(info) << "No words recognized in image" << std::endl;
		}
		else {
			// Get OCR result
			tineye.fontSizeCheck(*media, textBoxes);
			tineye.textContrastCheck(*media, textBoxes);
		}
	} while (media->nextFrame());

	tin::Results* results = media->getResultsPointer();
	std::cout << "SIZE: " << ((results->overallSizePass) ? "PASS" : "FAIL") <<
		"\tCONTRAST: " << ((results->overallContrastPass) ? "PASS" : "FAIL") << std::endl;

	//if specified in config, save outlines for both size and contrast
	if (config.getAppSettings()->saveTexboxOutline()) {
		media->saveResultsOutlines(results->contrastResults, "contrastChecks.png");
		media->saveResultsOutlines(results->sizeResults, "sizeChecks.png");
	}

	delete media;
}

void processFolder(tin::TinEye& tineye, fs::path path, tin::Configuration& config) {
	for (const auto& directoryEntry : fs::directory_iterator(path)) {
		if (fs::is_regular_file(directoryEntry)) {
			processMedia(tineye, directoryEntry, config);
		}
		//Ignore output results
		else if (fs::is_directory(directoryEntry)) {
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
	if (argc < 2) {
		std::cout << "Usage: \"./tineye_cli media_path/ [options] \n"
			<< "options:\n"
			<< "--east: running OCR on regions where EAST finds text";
		std::cin.get();
		return 1;
	}
	boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%Severity%] %Message%");

	BOOST_LOG_TRIVIAL(trace) << "Executing in " << std::filesystem::current_path() << std::endl;

	tin::TinEye tineye = tin::TinEye();
	tin::Configuration config = tin::Configuration("config.json");
	tineye.init(&config);

	fs::path path(argv[1]);

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


}