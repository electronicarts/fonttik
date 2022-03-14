#pragma once
#include <filesystem>
#include "Image.h"
#include "Textbox.h"

namespace fs = std::filesystem;

namespace tin {
	class Configuration;

	class TinEye {
		Configuration* config = nullptr;
		cv::dnn::TextRecognitionModel model;

		bool textboxSizeCheck(const Textbox& textbox);
		bool textboxContrastCheck(const Textbox& textbox, Image& image);
	public:
		TinEye() {};
		~TinEye();
		void init(fs::path configFile);

		std::vector<Textbox> getTextBoxes(Image& image);
		void mergeTextBoxes(std::vector<Textbox>& textBoxes);

		void applyFocusMask(Image& image);

		bool fontSizeCheck(Image& img, std::vector<Textbox>& boxes);
		bool textContrastCheck(Image& image, std::vector<Textbox>& boxes);
	};
}