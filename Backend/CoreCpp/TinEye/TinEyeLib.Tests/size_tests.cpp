#include <gtest/gtest.h>
#include "TinEye.h"

namespace tin {
	//All passing tests should pass with default configuration 
	TEST(SizeTests, 720Pass) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		//Open input image with openCV
		Image img;
		img.loadImage("resources/sizes/PassingSizeTest720.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye->applyFocusMask(img);
		std::vector<Textbox> textBoxes = tineye->getTextBoxes(img);
		tineye->mergeTextBoxes(textBoxes);

		passesSize = tineye->fontSizeCheck(img, textBoxes);

		delete tineye;

		ASSERT_TRUE(passesSize);
	}

	TEST(SizeTests, 1080Pass) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		//Open input image with openCV
		Image img;
		img.loadImage("resources/sizes/PassingSizeTest1080.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye->applyFocusMask(img);
		std::vector<Textbox> textBoxes = tineye->getTextBoxes(img);
		tineye->mergeTextBoxes(textBoxes);

		passesSize = tineye->fontSizeCheck(img, textBoxes);

		delete tineye;

		ASSERT_TRUE(passesSize);
	}

	TEST(SizeTests, 4kPass) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		//Open input image with openCV
		Image img;
		img.loadImage("resources/sizes/PassingSizeTest4k.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye->applyFocusMask(img);
		std::vector<Textbox> textBoxes = tineye->getTextBoxes(img);
		tineye->mergeTextBoxes(textBoxes);

		passesSize = tineye->fontSizeCheck(img, textBoxes);

		delete tineye;

		ASSERT_TRUE(passesSize);
	}

	//All failing tests should fail with default configuration
	TEST(SizeTests, 720Fail) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		//Open input image with openCV
		Image img;
		img.loadImage("resources/sizes/NotPassingSizeTest720.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye->applyFocusMask(img);
		std::vector<Textbox> textBoxes = tineye->getTextBoxes(img);
		tineye->mergeTextBoxes(textBoxes);

		passesSize = tineye->fontSizeCheck(img, textBoxes);

		delete tineye;

		ASSERT_FALSE(passesSize);
	}

	TEST(SizeTests, 1080Fail) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		//Open input image with openCV
		Image img;
		img.loadImage("resources/sizes/NotPassingSizeTest1080.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye->applyFocusMask(img);
		std::vector<Textbox> textBoxes = tineye->getTextBoxes(img);
		tineye->mergeTextBoxes(textBoxes);

		passesSize = tineye->fontSizeCheck(img, textBoxes);

		delete tineye;

		ASSERT_FALSE(passesSize);
	}

	TEST(SizeTests, 4kFail) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		//Open input image with openCV
		Image img;
		img.loadImage("resources/sizes/NotPassingSizeTest4k.png");

		bool passesSize = false;
		//Check if image has text recognized by OCR
		tineye->applyFocusMask(img);
		std::vector<Textbox> textBoxes = tineye->getTextBoxes(img);
		tineye->mergeTextBoxes(textBoxes);

		passesSize = tineye->fontSizeCheck(img, textBoxes);

		delete tineye;

		ASSERT_FALSE(passesSize);
	}
}