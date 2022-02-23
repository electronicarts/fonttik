#include "TinEye.h"
#include <iostream>
#include <algorithm>
#include "TextboxDetection.h"


bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: \"./tineye_cli media_path/ [options] \n"
			<< "options:\n"
			<< "--east: running OCR on regions where EAST finds text";
		std::cin.get();
		return 1;
	}
	TinEye* tineye = new TinEye();
	tineye->init("./config.json");

	//Open input image with openCV
	Image img;
	img.loadImage(argv[1]);

	bool pass  = false;
	bool EASTBoxes = cmdOptionExists(argv, argv + argc, "--east");

	do {
		if (EASTBoxes) {


			//BOOST_LOG_TRIVIAL(debug) << "Using EAST preprocessing" << std::endl;
			//Check if image has text recognized by OCR
			std::vector<std::vector<cv::Point>> textBoxes = TextboxDetection::detectBoxes(img.getImageMatrix(), false);

			if (textBoxes.empty()) {
				//BOOST_LOG_TRIVIAL(info) << "No words recognized in image" << std::endl;
			}
			else {
				// Get OCR result
				pass = tineye->fontSizeCheck(img, textBoxes);
			}

		}
		else {
			pass = tineye->fontSizeCheck(img);
		}
	} while (img.nextFrame());



	delete tineye;

	std::cout << ((pass) ? "PASS" : "FAIL") << std::endl;;
	std::cin.get();
}