#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	class ContrastRatioChecks : public ::testing::Test {
	protected:
		void SetUp() override {
			tineye = TinEye();
			config = Configuration("config.json");
			tineye.init(&config);
			str = "setup";
		}

		bool checkContrast(fs::path path) {
			img = Media::CreateMedia(path);

			//get matrix to know size of the image to be tested
			cv::Mat matrix = img->getImageMatrix();
			bool a = matrix.empty();
			//ASSERT_FALSE(matrix.empty());

			std::vector<Textbox> textBoxes;
			textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));
			bool contrastPass = tineye.textContrastCheck(*img, textBoxes);

			delete img;

			return contrastPass;
		}

		TinEye tineye;
		Media* img;
		Configuration config;
		std::string str = "default";
	};

	TEST_F(ContrastRatioChecks, PassingContrastFlat) {

		SetUp();
		std::string path = "resources/Contrasts/flatPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, PassingContrastGradient) {
		std::string path = "resources/Contrasts/gradientPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, PassingContrastStripes) {
		std::string path = "resources/Contrasts/stripesPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, FailingContrastFlat) {
		std::string path = "resources/Contrasts/flatFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, FailingContrastGradient) {
		std::string path = "resources/Contrasts/gradientFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, FailingContrastStripes) {
		std::string path = "resources/Contrasts/stripesFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, HighContrast) {
		std::string path = "resources/Contrasts/highContrast.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, lowContrast) {
		std::string path = "resources/Contrasts/lowContrast.png";
		img = Media::CreateMedia(path);
		ASSERT_FALSE(checkContrast(path));
	}
}