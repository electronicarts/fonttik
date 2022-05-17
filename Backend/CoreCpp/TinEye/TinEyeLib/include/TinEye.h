#pragma once
#include <filesystem>
#include "Media.h"
#include "Textbox.h"
#include <gtest/gtest.h>
#include <boost/log/utility/setup/file.hpp>

namespace fs = std::filesystem;
namespace logging = boost::log;
typedef logging::sinks::synchronous_sink<logging::sinks::text_file_backend> sink_t;

namespace tin {
	class Configuration;
	class TextboxDetection;
	class IChecker;

	class TinEye {
		boost::shared_ptr<sink_t> logSink;
		Configuration* config = nullptr;
		TextboxDetection* textboxDetection = nullptr;
		cv::dnn::TextRecognitionModel textRecognition;

		bool textboxSizeCheck(Media& image, Textbox& textbox);
		bool textboxContrastCheck(Media& image, Textbox& textbox);

		IChecker* contrastChecker;
		IChecker* sizeChecker;

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

		//Operator method
		//Calculates the contrast ratio of two given regions of a luminance matrix
		static double ContrastBetweenRegions(const cv::Mat& luminance, const cv::Mat& maskA, const cv::Mat& maskB);
	};
}