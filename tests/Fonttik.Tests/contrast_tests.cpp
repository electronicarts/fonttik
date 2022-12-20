//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "fonttik/Fonttik.h"
#include "fonttik/Configuration.h"
#include "fonttik/Log.h"

namespace tik {
	class ContrastRatioChecks : public ::testing::Test {
	protected:
		void SetUp() override {
			fonttik = Fonttik();
			config = Configuration("config/config_resolution.json");
			tik::Log::InitCoreLogger(false, false);
			fonttik.init(&config);
		}

		bool checkContrast(fs::path path) {
			img = Media::CreateMedia(path);
			Frame* frame = img->getFrame();

			//get matrix to know size of the image to be tested
			cv::Mat matrix = frame->getImageMatrix();
			
			std::vector<Textbox> textBoxes;
			textBoxes.emplace_back(cv::Rect(0, 0, matrix.cols, matrix.rows));


			FrameResults res= fonttik.textContrastCheck(*frame, textBoxes);

			delete img;
			delete frame;

			return res.overallPass;
		}

		Fonttik fonttik;
		Media* img;
		Configuration config;
	};

	TEST_F(ContrastRatioChecks, PassingContrastFlat) {

		std::string path = "config/Contrasts/flatPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, PassingContrastGradient) {
		std::string path = "config/Contrasts/gradientPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, PassingContrastStripes) {
		std::string path = "config/Contrasts/stripesPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, FailingContrastFlat) {
		std::string path = "config/Contrasts/flatFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, FailingContrastGradient) {
		std::string path = "config/Contrasts/gradientFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, FailingContrastStripes) {
		std::string path = "config/Contrasts/stripesFail.png";
		ASSERT_FALSE(checkContrast(path));

	}

	TEST_F(ContrastRatioChecks, HighContrast) {
		std::string path = "config/Contrasts/highContrast.png";
		ASSERT_TRUE(checkContrast(path));
	}

	TEST_F(ContrastRatioChecks, lowContrast) {
		std::string path = "config/Contrasts/lowContrast.png";
		ASSERT_FALSE(checkContrast(path));
	}
}