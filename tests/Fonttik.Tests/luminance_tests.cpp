//copyright (c) 2022 electronic arts, inc.  all rights reserved.

#include <gtest/gtest.h>
#include "fonttik/Media.hpp"
#include "fonttik/Fonttik.hpp"
#include "fonttik/Configuration.hpp"
#include "fonttik/Log.h"
#include "../../src/ContrastChecker.hpp"

namespace tik {
	class LuminanceTests : public ::testing::Test {
	protected:
		void SetUp() override {
			config = Configuration("config/config_resolution.json");
			tik::Log::InitCoreLogger(false, false);
			fonttik.init(&config);

			whiteMedia = Media::createMedia("config/luminance/white.png");
			whiteMedia->loadFrame();
			blackWhiteMedia = Media::createMedia("config/luminance/blackWhite.png");
			blackWhiteMedia->loadFrame();

			auto whiteImg = whiteMedia->getFrame();
			auto blackWhiteImg = blackWhiteMedia->getFrame();
			
			luminanceMapBlackWhite = getLuminance(blackWhiteImg.getFrameMat());
			luminanceMapWhite = getLuminance(whiteImg.getFrameMat());
		}

		cv::Mat getLuminance(cv::Mat input, cv::Rect crop = {}) {
			if (crop.empty())
			{
				crop = cv::Rect{ 0,0,input.rows,input.cols };
			}
			tik::TextBox screen = { crop, input };

			screen.calculateTextBoxLuminance(config.getSbgrValues());

			return screen.getTextMatLuminance();
		}

		void TearDown() override {
			delete blackWhiteMedia;
			delete whiteMedia;
		}

		Fonttik fonttik;
		Configuration config;
		Media* whiteMedia, * blackWhiteMedia;

		cv::Mat luminanceMapWhite;
		cv::Mat luminanceMapBlackWhite;
	};


	//White Luminance is 1
	TEST_F(LuminanceTests, WhiteImage) {

		cv::Mat mask = cv::Mat::ones({ luminanceMapWhite.cols, luminanceMapWhite.rows }, CV_8UC1);

		double mean = cv::mean(luminanceMapWhite, mask)[0];

		ASSERT_DOUBLE_EQ(mean, 1);
	}

	//Mean of both images is different
	//Mean of left half of blackWhite is the same as white
	TEST_F(LuminanceTests, ApplyMask) {

		cv::Mat mask = cv::Mat::zeros({ luminanceMapBlackWhite.cols, luminanceMapBlackWhite.rows }, CV_8UC1);
		double meanTwoHalves = cv::mean(luminanceMapBlackWhite, mask)[0];

		cv::Mat half = mask({ 0,0,luminanceMapBlackWhite.cols / 2,luminanceMapBlackWhite.rows });
		cv::bitwise_not(half, half);

		//White half mean
		double meanHalf = cv::mean(luminanceMapBlackWhite, mask)[0];

		//White image
		mask = cv::Mat::ones({ luminanceMapWhite.cols, luminanceMapWhite.rows }, CV_8UC1);
		double meanWhite = cv::mean(luminanceMapWhite, mask)[0];

		ASSERT_DOUBLE_EQ(meanHalf, meanWhite);
		ASSERT_NE(meanWhite, meanTwoHalves);
	}

	//Black on White has a contrast of 21 according to current legal procedure https://snook.ca/technical/colour_contrast/colour.html#fg=FFFFFF,bg=000000
	TEST_F(LuminanceTests, MaxContrast) {
		cv::Size size = { luminanceMapBlackWhite.cols,luminanceMapBlackWhite.rows };

		cv::Mat a = cv::Mat::zeros(size, CV_8UC1);
		cv::Mat halfA = a({ 0,0,size.width / 2,size.height });
		cv::bitwise_not(halfA, halfA);

		cv::Mat b;
		cv::bitwise_not(a, b);

		double contrast = ContrastChecker::getContrastBetweenRegions(luminanceMapBlackWhite, a, b);

		ASSERT_DOUBLE_EQ(contrast, 21);

	}

	//The order of the regions should not affect the contrast ratio
	TEST_F(LuminanceTests, Commutative) {
		cv::Size size = { luminanceMapBlackWhite.cols,luminanceMapBlackWhite.rows };

		cv::Mat a = cv::Mat::zeros(size, CV_8UC1);
		cv::Mat halfA = a({ 0,0,size.width / 2,size.height });
		cv::bitwise_not(halfA, halfA);

		cv::Mat b;
		cv::bitwise_not(a, b);

		double contrastA = ContrastChecker::getContrastBetweenRegions(luminanceMapBlackWhite, a, b);
		double contrastB = ContrastChecker::getContrastBetweenRegions(luminanceMapBlackWhite, b, a);

		ASSERT_DOUBLE_EQ(contrastA, contrastB);
	}
}