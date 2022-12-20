//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "fonttik/Fonttik.h"
#include "fonttik/Configuration.h"
#include "fonttik/Log.h"

namespace tik {
	class FonttikTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			fonttik = tik::Fonttik();
			config = tik::Configuration("config/config_resolution.json");
			fonttik.init(&config);

			fonttikDB = tik::Fonttik();
			configDB = tik::Configuration("config/config_resolution_DB.json");
			fonttikDB.init(&configDB);
		}

		Fonttik fonttik;
		Fonttik fonttikDB;
		Configuration config;
		Configuration configDB;
	};

	TEST_F(FonttikTests, PassingContrast) {
		tik::Media* media = tik::Media::CreateMedia("config/Contrasts/highContrast.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_TRUE(results->contrastPass());
	
		delete media;
	}

	TEST_F(FonttikTests, PassingContrastDB) {
		tik::Media* media = tik::Media::CreateMedia("config/Contrasts/highContrast.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_TRUE(results->contrastPass());

		delete media;
	}

	TEST_F(FonttikTests, FailingContrast) {
		tik::Media* media = tik::Media::CreateMedia("config/Contrasts/lowContrast.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_FALSE(results->contrastPass());

		delete media;
	}

	TEST_F(FonttikTests, FailingContrastDB) {
		tik::Media* media = tik::Media::CreateMedia("config/Contrasts/lowContrast.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_FALSE(results->contrastPass());

		delete media;
	}

	TEST_F(FonttikTests, PassingSize) {
		tik::Media* media = tik::Media::CreateMedia("config/sizes/720SerifPass.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_TRUE(results->sizePass());
	
		delete media;
	}
	
	TEST_F(FonttikTests, PassingSizeDB) {
		tik::Media* media = tik::Media::CreateMedia("config/sizes/720SerifPass.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_TRUE(results->sizePass());

		delete media;
	}

	TEST_F(FonttikTests, FailingSize) {
		tik::Media* media = tik::Media::CreateMedia("config/sizes/720SerifFail.png");

		tik::Results* results = fonttik.processMedia(*media);

		ASSERT_FALSE(results->sizePass());
	
		delete media;
	}

	TEST_F(FonttikTests, FailingSizeDB) {
		tik::Media* media = tik::Media::CreateMedia("config/sizes/720SerifFail.png");

		tik::Results* results = fonttikDB.processMedia(*media);

		ASSERT_FALSE(results->sizePass());

		delete media;
	}
}