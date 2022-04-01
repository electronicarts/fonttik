#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	class TinEyeTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tineye = tin::TinEye();
			config = tin::Configuration("config.json");
			tineye.init(&config);
		}

		TinEye tineye;
		Configuration config;
	};

	TEST_F(TinEyeTests, PassingContrast) {
		tin::Media* media = tin::Media::CreateMedia("resources/Contrasts/highContrast.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallContrastPass);
	
		delete media;
	}

	TEST_F(TinEyeTests, FailingContrast) {
		tin::Media* media = tin::Media::CreateMedia("resources/Contrasts/lowContrast.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_FALSE(results->overallContrastPass);

		delete media;
	}

	TEST_F(TinEyeTests, PassingSize) {
		tin::Media* media = tin::Media::CreateMedia("resources/sizes/720SerifPass.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallSizePass);
	
		delete media;
	}

	TEST_F(TinEyeTests, FailingSize) {
		tin::Media* media = tin::Media::CreateMedia("resources/sizes/720SerifFail.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_FALSE(results->overallSizePass);
	
		delete media;
	}
}