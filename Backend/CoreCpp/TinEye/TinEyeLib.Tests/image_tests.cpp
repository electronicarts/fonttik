#include <gtest/gtest.h>
#include "Image.h"

namespace tin {
	//All flipped pixels should be max luminance(255)-original value
	TEST(LuminanceFlip, FlipImage) {
		Image image;
		image.loadImage("resources/bf2042/chat_window_closed.png");
		cv::Mat original = image.getLuminanceMap().clone();
		image.flipLuminance();
		cv::Mat flipped = image.getLuminanceMap().clone();
		for (int i = 0; i < original.rows; i++) {
			for (int j = 0; j < original.cols; j++) {
				ASSERT_EQ(255 - original.at<uchar>(i, j), flipped.at<uchar>(i, j));
			}
		}
	}
	//All flipped pixels inside region should be max luminance(255)-original value
	//Outer pixels should remain unchanged
	TEST(LuminanceFlip, FlipRegion) {
		Image image;
		image.loadImage("resources/bf2042/chat_window_closed.png");
		cv::Mat original = image.getLuminanceMap().clone();
		int x1 = 100, y1 = 100, x2 = 300, y2 = 400;
		image.flipLuminance(x1, y1, x2, y2);
		cv::Mat flipped = image.getLuminanceMap().clone();

		for (int i = 0; i < original.rows; i++) {
			for (int j = 0; j < original.cols; j++) {
				if (i >= y1 && i <= y2 && j >= x1 && j <= x2) {
					ASSERT_EQ(255 - original.at<uchar>(i, j), flipped.at<uchar>(i, j)) << "Pixel not inverted: " << i << " " << j << std::endl;

				}
				else {
					ASSERT_EQ(original.at<uchar>(i, j), flipped.at<uchar>(i, j)) << "Pixel inverted: " << i << " " << j << std::endl;
				}
			}
		}
	}


	//Image should remain unchaged if luminance is flipped twice
	TEST(LuminanceFlip, FlipTwice) {
		Image image;
		image.loadImage("resources/bf2042/chat_window_closed.png");
		cv::Mat original = image.getLuminanceMap().clone();
		image.flipLuminance();
		image.flipLuminance();
		cv::Mat doubleFlip = image.getLuminanceMap().clone();
		ASSERT_TRUE(std::equal(original.begin<uchar>(), original.end<uchar>(), doubleFlip.begin<uchar>()));
	}

	//A region should remain unchaged if luminance is flipped twice
	TEST(LuminanceFlip, FlipTwiceRegion) {
		Image image;
		cv::Rect region(0, 0, 150, 150);
		image.loadImage("resources/bf2042/chat_window_closed.png");
		cv::Mat original = image.getLuminanceMap()(region).clone();
		image.flipLuminance(region.x, region.y, region.x + region.width, region.y + region.height);
		image.flipLuminance(region.x, region.y, region.x + region.width, region.y + region.height);
		cv::Mat doubleFlip = image.getLuminanceMap()(region).clone();
		ASSERT_TRUE(std::equal(original.begin<uchar>(), original.end<uchar>(), doubleFlip.begin<uchar>()));
	}
}
