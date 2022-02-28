#pragma once
#include <filesystem>
#include "Image.h"
#include "Textbox.h"

namespace fs = std::filesystem;

namespace tesseract {
	class TessBaseAPI;
}

class Configuration;

class TinEye {
	Configuration* config = nullptr;
	tesseract::TessBaseAPI* api = nullptr;
public:
	TinEye() {};
	~TinEye();
	void init(fs::path configFile);

	std::vector<Textbox> getTextBoxes(Image& image);

	bool fontSizeCheck(Image& img, std::vector<Textbox>& boxes);
	bool textContrastCheck(Image& image, std::vector<Textbox>& boxes);
};
