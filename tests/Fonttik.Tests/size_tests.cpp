//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "fonttik/Fonttik.hpp"
#include "fonttik/Configuration.hpp"
#include "fonttik/Media.hpp"
#include "fonttik/Log.h"

namespace tik {
	class SizeTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			config = Configuration("config/config_resolution.json");
			fonttik.init(&config);
		}

		bool passesSize(fs::path path) {
			Media* img = Media::createMedia(path.string());
			auto results = fonttik.processMedia(*img).getSizeResults();
			auto ret = std::all_of(results.begin(), results.end(), 
				[](const tik::FrameResults& result) {
					return std::all_of(result.results.begin(), result.results.end(),
						[](const auto& result) {return result.type == tik::ResultType::PASS; });
				});
			delete img;
			return ret;
		}

		Fonttik fonttik;
		Configuration config;
	};

	//All passing tests should pass with default configuration 
	TEST_F(SizeTests, 720SerifPass) {
		ASSERT_TRUE(passesSize("config/sizes/720SerifPass.png"));
	}

	TEST_F(SizeTests, 720SansPass) {
		ASSERT_TRUE(passesSize("config/sizes/720SansPass.png"));
	}

	TEST_F(SizeTests, 720MonospacePass) {
		ASSERT_TRUE(passesSize("config/sizes/720MonospacePass.png"));
	}

	TEST_F(SizeTests, 720BoldPass) {
		ASSERT_TRUE(passesSize("config/sizes/720BoldPass.png"));
	}

	TEST_F(SizeTests, 720ThinPass) {
		ASSERT_TRUE(passesSize("config/sizes/720ThinPass.png"));
	}

	TEST_F(SizeTests, 1080SerifPass) {
		ASSERT_TRUE(passesSize("config/sizes/1080SerifPass.png"));
	}

	TEST_F(SizeTests, 1080SansPass) {
		ASSERT_TRUE(passesSize("config/sizes/1080SansPass.png"));
	}

	TEST_F(SizeTests, 1080MonospacePass) {
		ASSERT_TRUE(passesSize("config/sizes/1080MonospacePass.png"));
	}

	TEST_F(SizeTests, 1080BoldPass) {
		ASSERT_TRUE(passesSize("config/sizes/1080BoldPass.png"));
	}

	TEST_F(SizeTests, 1080ThinPass) {
		ASSERT_TRUE(passesSize("config/sizes/1080ThinPass.png"));
	}

	TEST_F(SizeTests, 4kSerifPass) {
		ASSERT_TRUE(passesSize("config/sizes/4kSerifPass.png"));
	}

	TEST_F(SizeTests, 4kSansPass) {
		ASSERT_TRUE(passesSize("config/sizes/4kSansPass.png"));
	}

	TEST_F(SizeTests, 4kMonospacePass) {
		ASSERT_TRUE(passesSize("config/sizes/4kMonospacePass.png"));
	}

	TEST_F(SizeTests, 4kBoldPass) {
		ASSERT_TRUE(passesSize("config/sizes/4kBoldPass.png"));
	}

	TEST_F(SizeTests, 4kThinPass) {
		ASSERT_TRUE(passesSize("config/sizes/4kThinPass.png"));
	}


}