#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	TEST(ContrastRatioChecks, PassingContrastFlat) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);


		//Open input image with openCV
		std::string path = "resources/Contrasts/flatPass.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));
		ASSERT_TRUE(tineye.textContrastCheck(*img, textBoxes));

		delete img;
	}

	TEST(ContrastRatioChecks, PassingContrastGradient) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		std::string path = "resources/Contrasts/gradientPass.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_TRUE(tineye.textContrastCheck(*img, textBoxes));

		delete img;
	}

	TEST(ContrastRatioChecks, PassingContrastStripes) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		std::string path = "resources/Contrasts/stripesPass.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_TRUE(tineye.textContrastCheck(*img, textBoxes));

		delete img;
	}

	TEST(ContrastRatioChecks, FailingContrastFlat) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);


		//Open input image with openCV
		std::string path = "resources/Contrasts/flatFail.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye.textContrastCheck(*img, textBoxes));

		delete img;
	}

	TEST(ContrastRatioChecks, FailingContrastGradient) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		std::string path = "resources/Contrasts/gradientFail.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye.textContrastCheck(*img, textBoxes));
	}

	TEST(ContrastRatioChecks, FailingContrastStripes) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);


		//Open input image with openCV
		std::string path = "resources/Contrasts/stripesFail.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye.textContrastCheck(*img, textBoxes));

		delete img;
	}

	TEST(ContrastRatioChecks, HighContrast) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		std::string path = "resources/Contrasts/highContrast.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_TRUE(tineye.textContrastCheck(*img, textBoxes));

		delete img;
	}

	TEST(ContrastRatioChecks, lowContrast) {
		TinEye tineye = TinEye();
		Configuration config = Configuration("config.json");
		tineye.init(&config);

		//Open input image with openCV
		std::string path = "resources/Contrasts/lowContrast.png";
		Media* img = Media::CreateMedia(path);

		//get matrix to know size of the image to be tested
		cv::Mat matrix = img->getImageMatrix();

		std::vector<Textbox> textBoxes;
		textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));

		ASSERT_FALSE(tineye.textContrastCheck(*img, textBoxes));

		delete img;
	}
}