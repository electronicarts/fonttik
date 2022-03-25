#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	//All passing tests should pass with default configuration 
	TEST(SizeTests, 720Pass) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		Media* img = Media::CreateMedia("resources / sizes / PassingSizeTest720.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(*img);
		std::vector<Textbox> textBoxes = tineye.getTextBoxes(*img);
		tineye.mergeTextBoxes(textBoxes);

		passesSize = tineye.fontSizeCheck(*img, textBoxes);

		ASSERT_TRUE(passesSize);

		delete img;
	}

	TEST(SizeTests, 1080Pass) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		Media* img = Media::CreateMedia("resources/sizes/PassingSizeTest1080.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(*img);
		std::vector<Textbox> textBoxes = tineye.getTextBoxes(*img);
		tineye.mergeTextBoxes(textBoxes);

		passesSize = tineye.fontSizeCheck(*img, textBoxes);

		ASSERT_TRUE(passesSize);

		delete img;
	}

	TEST(SizeTests, 4kPass) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		Media* img = Media::CreateMedia("resources/sizes/PassingSizeTest4k.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(*img);
		std::vector<Textbox> textBoxes = tineye.getTextBoxes(*img);
		tineye.mergeTextBoxes(textBoxes);

		passesSize = tineye.fontSizeCheck(*img, textBoxes);

		ASSERT_TRUE(passesSize);

		delete img;
	}

	//All failing tests should fail with default configuration
	TEST(SizeTests, 720Fail) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		Media* img = Media::CreateMedia("resources/sizes/NotPassingSizeTest720.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(*img);
		std::vector<Textbox> textBoxes = tineye.getTextBoxes(*img);
		tineye.mergeTextBoxes(textBoxes);

		passesSize = tineye.fontSizeCheck(*img, textBoxes);

		ASSERT_FALSE(passesSize);

		delete img;

	}

	TEST(SizeTests, 1080Fail) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		Media* img = Media::CreateMedia("resources/sizes/NotPassingSizeTest1080.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(*img);
		std::vector<Textbox> textBoxes = tineye.getTextBoxes(*img);
		tineye.mergeTextBoxes(textBoxes);

		passesSize = tineye.fontSizeCheck(*img, textBoxes);

		ASSERT_FALSE(passesSize);

		delete img;

	}

	TEST(SizeTests, 4kFail) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		Media* img = Media::CreateMedia("resources/sizes/NotPassingSizeTest4k.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye.applyFocusMask(*img);
		std::vector<Textbox> textBoxes = tineye.getTextBoxes(*img);
		tineye.mergeTextBoxes(textBoxes);

		passesSize = tineye.fontSizeCheck(*img, textBoxes);

		ASSERT_FALSE(passesSize);

		delete img;
	}
}