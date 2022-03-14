#include <gtest/gtest.h>
#include "TinEye.h"

namespace tin {
	TEST(ContrastRatioChecks, PassingContrastFlat) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/flatPass.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_TRUE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}

	TEST(ContrastRatioChecks, PassingContrastGradient) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/gradientPass.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_TRUE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}

	TEST(ContrastRatioChecks, PassingContrastStripes) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/stripesPass.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_TRUE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}

	TEST(ContrastRatioChecks, FailingContrastFlat) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/flatFail.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}

	TEST(ContrastRatioChecks, FailingContrastGradient) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/gradientFail.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}

	TEST(ContrastRatioChecks, FailingContrastStripes) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/stripesFail.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}

	TEST(ContrastRatioChecks, HighContrast) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/highContrast.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_TRUE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}

	TEST(ContrastRatioChecks, lowContrast) {
		TinEye* tineye = new TinEye();
		tineye->init("config.json");

		Image img;

		//Open input image with openCV
		std::string path = "resources/Contrasts/lowContrast.png";
		img.loadImage(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img.getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye->textContrastCheck(img, textBoxes));

		delete tineye;
	}
}