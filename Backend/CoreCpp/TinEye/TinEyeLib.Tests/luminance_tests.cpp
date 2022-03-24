#include <gtest/gtest.h>
#include "Image.h"
#include "TinEye.h"
#include "Configuration.h"

namespace tin {

	//White Luminance is 1
	TEST(LuminanceMean, WhiteImage) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		Image img;
		img.loadImage("resources/luminance/white.png");
		cv::Mat luminanceMap = img.getLuminanceMap();

		cv::Mat mask = cv::Mat::ones({ luminanceMap.cols, luminanceMap.rows }, CV_8UC1);

		double mean = Image::LuminanceMeanWithMask(luminanceMap, mask);

		ASSERT_DOUBLE_EQ(mean, 1);
	}

	//Mean of both images is different
	//Mean of left half of blackWhite is the same as white
	TEST(LuminanceMean, ApplyMask) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		Image imgHalf, imgWhite;
		imgHalf.loadImage("resources/luminance/blackWhite.png");
		imgWhite.loadImage("resources/luminance/white.png");

		cv::Mat luminanceMap = imgHalf.getLuminanceMap();

		cv::Mat mask = cv::Mat::zeros({ luminanceMap.cols, luminanceMap.rows }, CV_8UC1);

		//Black and white mean
		double meanTwoHalves = Image::LuminanceMeanWithMask(luminanceMap, mask);

		cv::Mat half = mask({ 0,0,luminanceMap.cols / 2,luminanceMap.rows });
		cv::bitwise_not(half, half);

		//White half mean
		double meanHalf = Image::LuminanceMeanWithMask(luminanceMap, mask);

		luminanceMap = imgWhite.getLuminanceMap();
		mask = cv::Mat::ones({ luminanceMap.cols, luminanceMap.rows }, CV_8UC1 );

		//White image
		double meanWhite = Image::LuminanceMeanWithMask(luminanceMap, mask);

		ASSERT_DOUBLE_EQ(meanHalf, meanWhite);
		ASSERT_NE(meanWhite, meanTwoHalves);
	}

	//Black on White has a contrast of 21 according to current legal procedure https://snook.ca/technical/colour_contrast/colour.html#fg=FFFFFF,bg=000000
	TEST(ContrastRegions, MaxContrast) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		Image img;
		img.loadImage("resources/luminance/blackWhite.png");

		cv::Mat luminanceMap = img.getLuminanceMap();
		cv::Size size = { luminanceMap.cols,luminanceMap.rows };

		cv::Mat a = cv::Mat::zeros(size, CV_8UC1);
		cv::Mat halfA = a({ 0,0,size.width / 2,size.height });
		cv::bitwise_not(halfA, halfA);

		cv::Mat b;
		cv::bitwise_not(a, b);

		double contrast = TinEye::ContrastBetweenRegions(luminanceMap, a, b);

		ASSERT_DOUBLE_EQ(contrast, 21);
	}

	//The order of the regions should not affect the contrast ratio
	TEST(ContrastRegions, Commutative) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		Image img;
		img.loadImage("resources/luminance/blackWhite.png");

		cv::Mat luminanceMap = img.getLuminanceMap();
		cv::Size size = { luminanceMap.cols,luminanceMap.rows };

		cv::Mat a = cv::Mat::zeros(size, CV_8UC1);
		cv::Mat halfA = a({ 0,0,size.width / 2,size.height });
		cv::bitwise_not(halfA, halfA);

		cv::Mat b;
		cv::bitwise_not(a, b);

		double contrastA = TinEye::ContrastBetweenRegions(luminanceMap, a, b);
		double contrastB = TinEye::ContrastBetweenRegions(luminanceMap, b, a);

		ASSERT_DOUBLE_EQ(contrastA, contrastB);
	}

}