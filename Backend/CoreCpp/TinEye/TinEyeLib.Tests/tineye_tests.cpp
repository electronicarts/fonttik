#include <gtest/gtest.h>
#include "TinEye.h"
#include "Configuration.h"

namespace tin {
	TEST(TinEyeTests, PassingContrast) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");
		tineye.init(&config);

		tin::Media* media = tin::Media::CreateMedia("resources/Contrasts/highContrast.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallContrastPass);
	
		delete media;
	}

	TEST(TinEyeTests, FailingContrast) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");
		tineye.init(&config);

		tin::Media* media = tin::Media::CreateMedia("resources/Contrasts/lowContrast.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_FALSE(results->overallContrastPass);

		delete media;
	}

	TEST(TinEyeTests, PassingSize) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");
		tineye.init(&config);

		tin::Media* media = tin::Media::CreateMedia("resources/sizes/PassingSizeTest720.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_TRUE(results->overallSizePass);
	
		delete media;
	}

	TEST(TinEyeTests, FailingSize) {
		tin::TinEye tineye = tin::TinEye();
		tin::Configuration config = tin::Configuration("config.json");
		tineye.init(&config);

		tin::Media* media = tin::Media::CreateMedia("resources/sizes/NotPassingSizeTest720.png");

		tin::Results* results = tineye.processMedia(*media);

		ASSERT_FALSE(results->overallSizePass);
	
		delete media;
	}
}