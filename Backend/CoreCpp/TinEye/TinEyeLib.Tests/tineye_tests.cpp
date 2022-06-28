#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	class TinEyeTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tineye = tin::TinEye();
			config = tin::Configuration("unit_test/config_resolution.json");
			tineye.init(&config);
		}

		TinEye tineye;
		Configuration config;
	};

	TEST_F(TinEyeTests, PassingContrast) {
		tin::Media* media = tin::Media::CreateMedia("unit_test/Contrasts/highContrast.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->contrastPass());
	
		delete media;
	}

	TEST_F(TinEyeTests, FailingContrast) {
		tin::Media* media = tin::Media::CreateMedia("unit_test/Contrasts/lowContrast.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_FALSE(results->contrastPass());

		delete media;
	}

	TEST_F(TinEyeTests, PassingSize) {
		tin::Media* media = tin::Media::CreateMedia("unit_test/sizes/720SerifPass.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->sizePass());
	
		delete media;
	}

	TEST_F(TinEyeTests, FailingSize) {
		tin::Media* media = tin::Media::CreateMedia("unit_test/sizes/720SerifFail.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_FALSE(results->sizePass());
	
		delete media;
	}
}