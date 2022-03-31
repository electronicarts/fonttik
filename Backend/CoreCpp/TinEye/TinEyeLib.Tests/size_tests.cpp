#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	class SizeTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tineye = TinEye();
			config = Configuration("config.json");
			tineye.init(&config);
		}

		bool passesSize(fs::path path) {
			//Open input image with openCV
			Media* img = Media::CreateMedia(path);

			bool passesSize = false;
			//Check if image has text recognized by OCR
			tineye.applyFocusMask(*img);
			std::vector<Textbox> textBoxes = tineye.getTextBoxes(*img);
			tineye.mergeTextBoxes(textBoxes);

			passesSize = tineye.fontSizeCheck(*img, textBoxes);

			delete img;

			return passesSize;
		}

		TinEye tineye;
		Configuration config;
	};

	//All passing tests should pass with default configuration 
	TEST_F(SizeTests, 720Pass) {
		ASSERT_TRUE(passesSize("resources/sizes/PassingSizeTest720.png"));
	}

	TEST_F(SizeTests, 1080Pass) {
		ASSERT_TRUE(passesSize("resources/sizes/PassingSizeTest1080.png"));
	}

	TEST_F(SizeTests, 4kPass) {
		ASSERT_TRUE(passesSize("resources/sizes/PassingSizeTest4k.png"));
	}

	//All failing tests should fail with default configuration
	TEST_F(SizeTests, 720Fail) {
		ASSERT_FALSE(passesSize("resources/sizes/NotPassingSizeTest720.png"));
	}

	TEST_F(SizeTests, 1080Fail) {
		ASSERT_FALSE(passesSize("resources/sizes/NotPassingSizeTest1080.png"));
	}

	TEST_F(SizeTests, 4kFail) {
		ASSERT_FALSE(passesSize("resources/sizes/NotPassingSizeTest4k.png"));
	}
}