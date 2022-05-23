#include <gtest/gtest.h>
#include "Media.h"
#include "Video.h"

namespace tin {
	class VideoTests : public ::testing::Test {
	protected:
		bool checkSimilarity(fs::path path) {
			tin::Video* video = static_cast<tin::Video*>(tin::Media::CreateMedia(path));

			cv::Mat nextFrame;
			video->videoCapture >> nextFrame;
			
			bool comparison = video->compareFramesSimilarity(video->imageMatrix, nextFrame);

			delete video;

			return comparison;
		}
	};

	TEST_F(VideoTests, HighSimilarity) {
		std::string path = "unit_test/Video/HighSimilarity.gif";
		ASSERT_TRUE(checkSimilarity(path));
	}

	TEST_F(VideoTests, LowSimilarity) {
		std::string path = "unit_test/Video/LowSimilarity.gif";
		ASSERT_FALSE(checkSimilarity(path));
	}
}