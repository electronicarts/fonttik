#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	class ContrastRatioChecks : public ::testing::Test {
	protected:
		void SetUp() override {
			tineye = TinEye();
			config = Configuration("unit_test/config_resolution.json");
			tineye.init(&config);
		}

		bool checkContrast(fs::path path) {
			img = Media::CreateMedia(path);

			//get matrix to know size of the image to be tested
			cv::Mat matrix = img->getImageMatrix();
			bool a = matrix.empty();
			
			std::vector<Textbox> textBoxes;
			textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));


			Frame* frame = img->getFrame();
			FrameResults res= tineye.textContrastCheck(*frame, textBoxes);

			delete img;
			delete frame;

			return res.pass;
		}

		TinEye tineye;
		Media* img;
		Configuration config;
	};

	TEST_F(ContrastRatioChecks, PassingContrastFlat) {

		std::string path = "unit_test/Contrasts/flatPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, PassingContrastGradient) {
		std::string path = "unit_test/Contrasts/gradientPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, PassingContrastStripes) {
		std::string path = "unit_test/Contrasts/stripesPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, FailingContrastFlat) {
		std::string path = "unit_test/Contrasts/flatFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, FailingContrastGradient) {
		std::string path = "unit_test/Contrasts/gradientFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, FailingContrastStripes) {
		std::string path = "unit_test/Contrasts/stripesFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, HighContrast) {
		std::string path = "unit_test/Contrasts/highContrast.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, lowContrast) {
		std::string path = "unit_test/Contrasts/lowContrast.png";
		ASSERT_FALSE(checkContrast(path));
	}
}