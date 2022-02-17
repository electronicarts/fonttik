#include "TinEye.h"
#include <iostream>
#include <algorithm>

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: \"./tineye_cli media_path \n"
			"options:\n"
			"--east: running OCR on regions where EAST finds text";
		return 1;
	}
	TinEye* tineye = new TinEye();
	tineye->init("./config.json");
	bool pass = tineye->fontSizeCheck(argv[1], cmdOptionExists(argv, argv + argc, "--east"));

	delete tineye;

	std::cout << ((pass) ? "PASS" : "FAIL") << std::endl;;
}