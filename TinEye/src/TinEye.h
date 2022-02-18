#pragma once
#include "Configuration.h"
#include <tesseract/baseapi.h>
#include "Image.h"
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

#ifdef _WIN
#define DLLExport   __declspec( dllexport )
#endif
#ifdef _UNIX
#define DLLExport  __attribute__((visibility("default")))
#endif

class DLLExport TinEye {
	Configuration config;
	tesseract::TessBaseAPI* api = nullptr;
	bool fontSizeCheck(Image& img);
	bool fontSizeCheck(Image& img, std::vector<std::vector<cv::Point>>& boxes);
public:
	TinEye() {};
	~TinEye();
	void init(fs::path configFile);
	bool fontSizeCheck(fs::path imagePath, bool EAST_Textboxes=false);
};
