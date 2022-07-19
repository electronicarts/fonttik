//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "Media.h"
#include "TinEye.h"
#include "Configuration.h"
#include "Log.h"

namespace tin {
	class LuminanceFlipTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tineye = TinEye();
			config = Configuration("unit_test/config_resolution.json");
			tin::Log::InitCoreLogger(false, false);
			tineye.init(&config);

			media = Media::CreateMedia("unit_test/luminance/chat_window_closed.png");

			image = media->getFrame();

		}

		void TearDown() override {
			delete media;
			delete image;
		}

		TinEye tineye;
		Configuration config;
		Media* media;
		Frame* image;
	};

	//All flipped pixels should be max luminance(255)-original value
	TEST_F(LuminanceFlipTests, FlipImage) {
		cv::Mat original = image->getFrameLuminance().clone();
		image->flipLuminance();
		cv::Mat flipped = image->getFrameLuminance().clone();
		for (int i = 0; i < original.rows; i++) {
			for (int j = 0; j < original.cols; j++) {
				ASSERT_EQ(1 - original.at<double>(i, j), flipped.at<double>(i, j));
			}
		}

	}
	//All flipped pixels inside region should be max luminance(255)-original value
	//Outer pixels should remain unchanged
	TEST_F(LuminanceFlipTests, FlipRegion) {
		cv::Mat original = image->getFrameLuminance().clone();
		int x1 = 100, y1 = 100, x2 = 300, y2 = 400;
		image->flipLuminance(x1, y1, x2, y2);
		cv::Mat flipped = image->getFrameLuminance().clone();

		for (int i = 0; i < original.rows; i++) {
			for (int j = 0; j < original.cols; j++) {
				if (i >= y1 && i <= y2 && j >= x1 && j <= x2) {
					ASSERT_EQ(1 - original.at<double>(i, j), flipped.at<double>(i, j)) << "Pixel not inverted: " << i << " " << j << std::endl;

				}
				else {
					ASSERT_EQ(original.at<double>(i, j), flipped.at<double>(i, j)) << "Pixel inverted: " << i << " " << j << std::endl;
				}
			}
		}

	}

	//Image should remain unchanged if luminance is flipped twice
	TEST_F(LuminanceFlipTests, FlipTwice) {
		cv::Mat original = image->getFrameLuminance().clone();
		image->flipLuminance();
		image->flipLuminance();
		cv::Mat doubleFlip = image->getFrameLuminance().clone();

		original.convertTo(original, CV_8UC1, 255);
		doubleFlip.convertTo(doubleFlip, CV_8UC1, 255);

		ASSERT_TRUE(std::equal(original.begin<uchar>(), original.end<uchar>(), doubleFlip.begin<uchar>()));
	
	}

	//A region should remain unchanged if luminance is flipped twice
	TEST_F(LuminanceFlipTests, FlipTwiceRegion) {
		
		cv::Rect region(0, 0, 150, 150);
		cv::Mat original = image->getFrameLuminance()(region).clone();
		image->flipLuminance(region.x, region.y, region.x + region.width, region.y + region.height);
		image->flipLuminance(region.x, region.y, region.x + region.width, region.y + region.height);
		cv::Mat doubleFlip = image->getFrameLuminance()(region).clone();
		
		original.convertTo(original, CV_8UC1, 255);
		doubleFlip.convertTo(doubleFlip, CV_8UC1, 255);

		ASSERT_TRUE(std::equal(original.begin<uchar>(), original.end<uchar>(), doubleFlip.begin<uchar>()));
	}
}
