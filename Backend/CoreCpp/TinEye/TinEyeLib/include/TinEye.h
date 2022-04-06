#pragma once
#include <filesystem>
#include "Media.h"
#include "Textbox.h"
#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace tin {
	class Configuration;
	class TextboxDetection;

	class TinEye {
		Configuration* config = nullptr;
		TextboxDetection* textboxDetection = nullptr;
		cv::dnn::TextRecognitionModel textRecognition;

		bool textboxSizeCheck(Media& image, Textbox& textbox);
		bool textboxContrastCheck(Media& image, Textbox& textbox);

		//Operator method
		//Calculates the contrast ratio of two given regions of a luminance matrix
		static double ContrastBetweenRegions(const cv::Mat& luminance, const cv::Mat& maskA, const cv::Mat& maskB);

		static std::vector<double>* linearizationLUT;
		static double linearize8bitRGB(const uchar& colorBits);

		FRIEND_TEST(LuminanceTests, MaxContrast);
		FRIEND_TEST(LuminanceTests, Commutative);
	public:
		TinEye() {};
		//Overloaded constructor that automatically calls init with provided configuration
		TinEye(Configuration* configuration) { init(configuration); }
		~TinEye();
		void init(Configuration* configuration);

		std::vector<Textbox> getTextBoxes(Media& image);
		void mergeTextBoxes(std::vector<Textbox>& textBoxes);

		void applyFocusMask(Media& image);

		bool fontSizeCheck(Media& img, std::vector<Textbox>& boxes);
		bool textContrastCheck(Media& image, std::vector<Textbox>& boxes);
		Results* processMedia(Media& media);

		//Operator
		//Calculates the luminance of a given image matrix
		static cv::Mat calculateLuminance(cv::Mat imageMatrix);
	};
}