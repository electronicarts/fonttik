//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "Fonttik.h"
#include "Configuration.h"
#include "Log.h"

namespace tik {
	class FonttikTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			fonttik = tik::Fonttik();
			config = tik::Configuration("unit_test/config_resolution.json");
			fonttik.init(&config);

			fonttikDB = tik::Fonttik();
			configDB = tik::Configuration("unit_test/config_resolution_DB.json");
			fonttikDB.init(&configDB);
		}

		Fonttik fonttik;
		Fonttik fonttikDB;
		Configuration config;
		Configuration configDB;
	};

	TEST_F(FonttikTests, PassingContrast) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/Contrasts/highContrast.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_TRUE(results->contrastPass());
	
		delete media;
	}

	TEST_F(FonttikTests, PassingContrastDB) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/Contrasts/highContrast.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_TRUE(results->contrastPass());

		delete media;
	}

	TEST_F(FonttikTests, FailingContrast) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/Contrasts/lowContrast.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_FALSE(results->contrastPass());

		delete media;
	}

	TEST_F(FonttikTests, FailingContrastDB) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/Contrasts/lowContrast.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_FALSE(results->contrastPass());

		delete media;
	}

	TEST_F(FonttikTests, PassingSize) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/sizes/720SerifPass.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_TRUE(results->sizePass());
	
		delete media;
	}
	
	TEST_F(FonttikTests, PassingSizeDB) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/sizes/720SerifPass.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_TRUE(results->sizePass());

		delete media;
	}

	TEST_F(FonttikTests, FailingSize) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/sizes/720SerifFail.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_FALSE(results->sizePass());
	
		delete media;
	}

	TEST_F(FonttikTests, FailingSizeDB) {
		tik::Media* media = tik::Media::CreateMedia("unit_test/sizes/720SerifFail.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_FALSE(results->sizePass());

		delete media;
	}
}