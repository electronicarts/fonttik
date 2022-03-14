#pragma once
#include <filesystem>
#include "Image.h"
#include "Textbox.h"
#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace tin {
	class Configuration;

	class TinEye {
		Configuration* config = nullptr;
		cv::dnn::TextRecognitionModel model;

		bool textboxSizeCheck(const Textbox& textbox);
		bool textboxContrastCheck(const Textbox& textbox, Image& image);
		bool textboxContrastCheck(const Textbox& textbox);

		static double ContrastBetweenRegions(const cv::Mat& luminance, const cv::Mat& maskA, const cv::Mat& maskB);

		FRIEND_TEST(ContrastRegions, MaxContrast);
		FRIEND_TEST(ContrastRegions, Commutative);
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