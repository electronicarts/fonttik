//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "fonttik/Fonttik.h"
#include "fonttik/Configuration.h"
#include "fonttik/Log.h"

namespace tik {
	class SizeTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			fonttik = Fonttik();
			config = Configuration("config/config_resolution.json");
			fonttik.init(&config);
		}

		bool passesSize(fs::path path) {
			//Open input image with openCV
			Media* img = Media::CreateMedia(path);

			Frame* frame = img->getFrame();

			//Check if image has text recognized by OCR
			fonttik.applyFocusMask(*frame);
			std::vector<Textbox> textBoxes = fonttik.getTextBoxes(*frame);
			fonttik.mergeTextBoxes(textBoxes);

			FrameResults res = fonttik.fontSizeCheck(*frame, textBoxes);

			delete img;
			delete frame;

			return res.overallPass;
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

	//All failing tests should fail with default configuration
	TEST_F(SizeTests, 720SerifFail) {
		ASSERT_FALSE(passesSize("config/sizes/720SerifFail.png"));
	}

	TEST_F(SizeTests, 720SansFail) {
		ASSERT_FALSE(passesSize("config/sizes/720SansFail.png"));
	}

	TEST_F(SizeTests, 720MonospaceFail) {
		ASSERT_FALSE(passesSize("config/sizes/720MonospaceFail.png"));
	}

	TEST_F(SizeTests, 720BoldFail) {
		ASSERT_FALSE(passesSize("config/sizes/720BoldFail.png"));
	}

	TEST_F(SizeTests, 720ThinFail) {
		ASSERT_FALSE(passesSize("config/sizes/720ThinFail.png"));
	}

	TEST_F(SizeTests, 1080SerifFail) {
		ASSERT_FALSE(passesSize("config/sizes/1080SerifFail.png"));
	}

	TEST_F(SizeTests, 1080SansFail) {
		ASSERT_FALSE(passesSize("config/sizes/1080SansFail.png"));
	}

	TEST_F(SizeTests, 1080MonospaceFail) {
		ASSERT_FALSE(passesSize("config/sizes/1080MonospaceFail.png"));
	}

	TEST_F(SizeTests, 1080BoldFail) {
		ASSERT_FALSE(passesSize("config/sizes/1080BoldFail.png"));
	}

	TEST_F(SizeTests, 1080ThinFail) {
		ASSERT_FALSE(passesSize("config/sizes/1080ThinFail.png"));
	}

	TEST_F(SizeTests, 4kSerifFail) {
		ASSERT_FALSE(passesSize("config/sizes/4kSerifFail.png"));
	}

	TEST_F(SizeTests, 4kSansFail) {
		ASSERT_FALSE(passesSize("config/sizes/4kSansFail.png"));
	}

	TEST_F(SizeTests, 4kMonospaceFail) {
		ASSERT_FALSE(passesSize("config/sizes/4kMonospaceFail.png"));
	}

	TEST_F(SizeTests, 4kBoldFail) {
		ASSERT_FALSE(passesSize("config/sizes/4kBoldFail.png"));
	}

	TEST_F(SizeTests, 4kThinFail) {
		ASSERT_FALSE(passesSize("config/sizes/4kThinFail.png"));
	}
}