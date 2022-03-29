#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	TEST(TinEyeTests, PassingContrast) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");

		tin::Media* media = tin::Media::CreateMedia("resources/contrasts/gradientPass.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallContrastPass);
	
		delete media;
	}

	TEST(TinEyeTests, FailingContrast) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");

		tin::Media* media = tin::Media::CreateMedia("resources/Contrasts/gradientFail.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallContrastPass);

		delete media;
	}

	TEST(TinEyeTests, PassingSize) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");

		tin::Media* media = tin::Media::CreateMedia("resources/sizes/PassingSizeTest720.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallContrastPass);
	
		delete media;
	}

	TEST(TinEyeTests, FailingSize) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");

		tin::Media* media = tin::Media::CreateMedia("resources/sizes/NotPassingSizeTest720.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallContrastPass);
	
		delete media;
	}
}