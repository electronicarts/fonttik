//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "Media.h"
#include "Fonttik.h"
#include "Configuration.h"
#include "Log.h"

namespace tik {
	class ResultsTests : public ::testing::Test {
	protected:
		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			fonttik = Fonttik();

			media = Media::CreateMedia("unit_test/sizes/1080BoldPass.png");
		}

		void TearDown() override {
			delete media;
		}

		bool compareResults(std::string path, std::vector<tik::FrameResults>& results, bool printValuesOnResults,std::vector<cv::Scalar> colors, std::string name = "checks") {
			media->saveResultsOutlines(results,
				media->getOutputPath() / name, colors,
				printValuesOnResults);

			cv::Mat result = cv::imread((media->getOutputPath() / (name + ".png")).string(), cv::IMREAD_COLOR);
			cv::Mat expected = cv::imread(path, cv::IMREAD_COLOR);

			return (cv::sum(result != expected) == cv::Scalar(0, 0, 0, 0));
		}

		Fonttik fonttik;
		Media* media;
	};

	TEST_F(ResultsTests, ContrastNoOverlay) {
		Configuration config = Configuration("unit_test/config_resolution.json");
		fonttik.init(&config);

		Results* res = fonttik.processMedia(*media);

		//Check that obtained results are the same as expected ones
		ASSERT_TRUE(compareResults("unit_test/sizes/contrastNoOverlay1080BoldPass.png", res->getContrastResults(),
			false, config.getAppSettings()->getColors(),"contrastChecks"));
	}


		
	TEST_F(ResultsTests, SizeNoOverlay) {
		Configuration config = Configuration("unit_test/config_resolution.json");
		fonttik.init(&config);

		Results* res = fonttik.processMedia(*media);

		//Check that obtained results are the same as expected ones
		ASSERT_TRUE(compareResults("unit_test/sizes/sizeNoOverlay1080BoldPass.png", 
			res->getSizeResults(), false,config.getAppSettings()->getColors(),"sizeChecks"));
	}



	TEST_F(ResultsTests, SizeOverlay) {
		Configuration config = Configuration("unit_test/config_withOverlay.json");
		fonttik.init(&config);

		Results* res = fonttik.processMedia(*media);

		//Check that obtained results are the same as expected ones
		ASSERT_TRUE(compareResults("unit_test/sizes/sizeOverlay1080BoldPass.png",
			res->getSizeResults(), true, config.getAppSettings()->getColors(),"sizeChecks"));
	}

	TEST_F(ResultsTests, ContrastOverlay) {
		Configuration config = Configuration("unit_test/config_withOverlay.json");
		fonttik.init(&config);

		Results* res = fonttik.processMedia(*media);

		//Check that obtained results are the same as expected ones
		ASSERT_TRUE(compareResults("unit_test/sizes/contrastOverlay1080BoldPass.png",
			res->getContrastResults(), true, config.getAppSettings()->getColors(), "contrastChecks"));
	}
}
