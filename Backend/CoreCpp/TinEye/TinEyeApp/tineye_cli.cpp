#include "TinEye.h"
#include <iostream>
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <filesystem>

namespace fs = std::filesystem;

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

void processImage(tin::TinEye& tineye, fs::path path) {
	tin::Image img;
	img.loadImage(path);

	do {


		BOOST_LOG_TRIVIAL(debug) << "Using EAST preprocessing" << std::endl;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(img);
		std::vector<tin::Textbox> textBoxes = tineye.getTextBoxes(img);
		tineye.mergeTextBoxes(textBoxes);
		if (textBoxes.empty()) {
			BOOST_LOG_TRIVIAL(info) << "No words recognized in image" << std::endl;
		}
		else {
			// Get OCR result
			tineye.fontSizeCheck(img, textBoxes);
			tineye.textContrastCheck(img, textBoxes);
		}
	} while (img.nextFrame());

	tin::Results* results = img.getResultsPointer();
	std::cout << "SIZE: " << ((results->overallSizePass) ? "PASS" : "FAIL") <<
		"\tCONTRAST: " << ((results->overallContrastPass) ? "PASS" : "FAIL") << std::endl;;
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
	//boost::log::core::get()->set_filter(
	//	boost::log::trivial::severity >= boost::log::trivial::warning
	//);

	BOOST_LOG_TRIVIAL(trace) << "Executing in " << std::filesystem::current_path() << std::endl;

	tin::TinEye tineye = tin::TinEye();
	tineye.init("config.json");

	fs::path path(argv[1]);

	if (fs::exists(path)) {

		if (!fs::is_directory(path)) {
			processImage(tineye, path);
		}
		else {
			for (auto const& directoryEntry : fs::recursive_directory_iterator(path)) {
				if (fs::is_regular_file(directoryEntry)) {
					processImage(tineye, directoryEntry);
				}
			}
		}
	}
	else {
		std::cerr << "Path not found " << std::endl;
	}


}