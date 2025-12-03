////Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.
//
//#include <gtest/gtest.h>
//#include <filesystem>
//#include <nlohmann/json.hpp>
//#include "fonttik/Fonttik.hpp"
//#include "fonttik/Configuration.hpp"
//#include "fonttik/Log.h"
//#include "fonttik/Media.hpp"
//
//namespace tik {
//	class FonttikTests : public ::testing::Test {
//	protected:
//		void SetUp() override {
//			tik::Log::InitCoreLogger(false, false);
//			config = tik::Configuration("config/config_resolution.json");
//			fonttik.init(&config);
//
//			configDB = tik::Configuration("config/config_resolution_DB.json");
//			fonttikDB.init(&configDB);
//		}
//
//		Fonttik fonttik;
//		Fonttik fonttikDB;
//		Configuration config;
//		Configuration configDB;
//	};
//
//	TEST_F(FonttikTests, PassingContrast) {
//		tik::Media* media = tik::Media::createMedia("config/Contrasts/highContrast.png");
//
//		tik::Results results = fonttik.processMedia(*media);
//
//		ASSERT_TRUE(results.contrastPass());
//	
//		delete media;
//	}
//
//	TEST_F(FonttikTests, PassingContrastDB) {
//		tik::Media* media = tik::Media::createMedia("config/Contrasts/highContrast.png");
//
//		tik::Results results = fonttikDB.processMedia(*media);
//
//		ASSERT_TRUE(results.contrastPass());
//
//		delete media;
//	}
//
//	TEST_F(FonttikTests, FailingContrast) {
//		tik::Media* media = tik::Media::createMedia("config/Contrasts/lowContrast.png");
//
//		tik::Results results = fonttik.processMedia(*media);
//
//		ASSERT_FALSE(results.contrastPass());
//
//		delete media;
//	}
//
//	TEST_F(FonttikTests, FailingContrastDB) {
//		tik::Media* media = tik::Media::createMedia("config/Contrasts/lowContrast.png");
//
//		tik::Results results = fonttikDB.processMedia(*media);
//
//		ASSERT_FALSE(results.contrastPass());
//
//		delete media;
//	}
//
//	TEST_F(FonttikTests, PassingSize) {
//		tik::Media* media = tik::Media::createMedia("config/sizes/720SerifPass.png");
//
//		tik::Results results = fonttik.processMedia(*media);
//
//		ASSERT_TRUE(results.sizePass());
//	
//		delete media;
//	}
//	
//	TEST_F(FonttikTests, PassingSizeDB) {
//		tik::Media* media = tik::Media::createMedia("config/sizes/720SerifPass.png");
//
//		tik::Results results = fonttikDB.processMedia(*media);
//
//		ASSERT_TRUE(results.sizePass());
//
//		delete media;
//	}
//
//	TEST_F(FonttikTests, FailingSize) {
//		tik::Media* media = tik::Media::createMedia("config/sizes/720SerifFail.png");
//
//		tik::Results results = fonttik.processMedia(*media);
//
//		ASSERT_FALSE(results.sizePass());
//	
//		delete media;
//	}
//
//	TEST_F(FonttikTests, FailingSizeDB) {
//		tik::Media* media = tik::Media::createMedia("config/sizes/720SerifFail.png");
//
//		tik::Results results = fonttikDB.processMedia(*media);
//
//		ASSERT_FALSE(results.sizePass());
//
//		delete media;
//	}
//
//	TEST_F(FonttikTests, AsyncTestImage) {
//		tik::Media* media = tik::Media::createMedia("config/sizes/720SerifFail.png");
//		auto results = fonttikDB.processMediaAsync(*media);
//		
//		auto sizePath = media->getOutputPath() / "sizeChecks.json";
//		ASSERT_TRUE(std::filesystem::is_regular_file(sizePath));
//		auto contrastPath = media->getOutputPath() / "contrastChecks.json";
//		ASSERT_TRUE(std::filesystem::is_regular_file(contrastPath));
//		
//		using json = nlohmann::json;
//		
//		std::ifstream iSize(sizePath);
//		json size = json::parse(iSize);
//
//		std::ifstream iContrast(contrastPath);
//		json contrast = json::parse(iContrast);
//
//		delete media;
//	}
//
//	TEST_F(FonttikTests, AsyncTestVideo) {
//		tik::Media* media = tik::Media::createMedia("config/Video/LowSimilarity.gif");
//		auto results = fonttikDB.processMediaAsync(*media);
//
//		auto sizePath = media->getOutputPath() / "sizeChecks.json";
//		ASSERT_TRUE(std::filesystem::is_regular_file(sizePath));
//		auto contrastPath = media->getOutputPath() / "contrastChecks.json";
//		ASSERT_TRUE(std::filesystem::is_regular_file(contrastPath));
//
//		using json = nlohmann::json;
//
//		std::ifstream iSize(sizePath);
//		json size = json::parse(iSize);
//		ASSERT_TRUE(size.size() >= 1); //Video results should include at least first frame
//
//		std::ifstream iContrast(contrastPath);
//		json contrast = json::parse(iContrast);
//		ASSERT_TRUE(contrast.size() >= 1);//Video results should include at least first frame
//
//		delete media;
//	}
//} 