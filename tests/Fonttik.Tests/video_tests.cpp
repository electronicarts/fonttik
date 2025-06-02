//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "fonttik/Media.hpp"
#include "../src/Video.hpp"
#include "fonttik/Results.h"
#include "fonttik/Log.h"

namespace tik {
	class VideoTests : public ::testing::Test {
	protected:
		bool checkSimilarity(fs::path path) {
			tik::Log::InitCoreLogger(false, false);
			tik::Video* video = static_cast<tik::Video*>(tik::Media::createMedia(path.string()));
			bool comparison = video->compareFramesSimilarity(video->_GetNextFrame(), video->_GetNextFrame());
			delete video;
			return comparison;
		}
	};

	TEST_F(VideoTests, HighSimilarity) {
		std::string path = "config/Video/HighSimilarity.gif";
		ASSERT_TRUE(checkSimilarity(path));
	}

	TEST_F(VideoTests, LowSimilarity) {
		std::string path = "config/Video/LowSimilarity.gif";
		ASSERT_FALSE(checkSimilarity(path));
	}

	class FrameSorting : public ::testing::Test {
	protected:
		Results r;
		void SetUp() override {
			int frameNumbers[4] = { 6,3,76,2 };
			for (const int& number:frameNumbers) {
				r.addContrastResults(number);
				r.addSizeResults(number);
			}
		}
	};

	TEST_F(FrameSorting, OrderedContrast) {
		auto contrastResults = r.getContrastResults();
		
		for (int i = 0; i < contrastResults.size() - 1; i++) {
			ASSERT_LT(contrastResults[i].frame, contrastResults[i + 1].frame);
		}
	}

	TEST_F(FrameSorting, OrderedContrast_After_Push) {
		auto contrastResults = r.getContrastResults();

		for (int i = 0; i < contrastResults.size() - 1; i++) {
			ASSERT_LT(contrastResults[i].frame, contrastResults[i + 1].frame);
		}

		r.addContrastResults(100);

		contrastResults = r.getContrastResults();

		for (int i = 0; i < contrastResults.size() - 1; i++) {
			ASSERT_LT(contrastResults[i].frame, contrastResults[i + 1].frame);
		}
	}

	TEST_F(FrameSorting, OrderedSize) {
		auto sizeResults = r.getSizeResults();

		for (int i = 0; i < sizeResults.size() - 1; i++) {
			ASSERT_LT(sizeResults[i].frame, sizeResults[i + 1].frame);
		}
	}

	TEST_F(FrameSorting, OrderedSize_After_Push) {
		auto sizeResults = r.getSizeResults();

		for (int i = 0; i < sizeResults.size() - 1; i++) {
			ASSERT_LT(sizeResults[i].frame, sizeResults[i + 1].frame);
		}

		r.addContrastResults(100);

		sizeResults = r.getSizeResults();

		for (int i = 0; i < sizeResults.size() - 1; i++) {
			ASSERT_LT(sizeResults[i].frame, sizeResults[i + 1].frame);
		}
	}
}