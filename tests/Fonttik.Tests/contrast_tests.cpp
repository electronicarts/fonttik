//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "fonttik/Fonttik.hpp"
#include "fonttik/Configuration.hpp"
#include "fonttik/Log.h"
#include "fonttik/Media.hpp"

namespace tik {
	class ContrastRatioChecks : public ::testing::Test {
	protected:
		void SetUp() override {
			config = Configuration("config/config_resolution.json");
			config.setTargetResolution("1080");
			tik::Log::InitCoreLogger(false, false);
			fonttik.init(&config);
		}

		bool checkContrast(fs::path path) {
			img = Media::createMedia(path.string());
			bool ret = fonttik.processMedia(*img).getContrastResults()[0].overallPass;
			delete img;
			return ret;
		}

		Fonttik fonttik;
		Media* img;
		Configuration config;
	};

	TEST_F(ContrastRatioChecks, PassingContrastFlat) {

		std::string path = "config/Contrasts/flatPass.png";
		ASSERT_TRUE(checkContrast(path));
	}

	/*TEST_F(ContrastRatioChecks, PassingContrastGradient) {
		std::string path = "config/Contrasts/gradientPass.png";
		ASSERT_TRUE(checkContrast(path));
	}*/

	/*TEST_F(ContrastRatioChecks, PassingContrastStripes) {
		std::string path = "config/Contrasts/stripesPass.png";
		ASSERT_TRUE(checkContrast(path));
	}*/

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