#include <gtest/gtest.h>
#include "fonttik/Log.h"
#include "fonttik/Fonttik.hpp"
#include "fonttik/Configuration.hpp"
#include "../../src/ColorblindFilters.hpp"

namespace tik {
    class ColorblindnessTests : public ::testing::Test {
    protected:
        void SetUp() override {
            config = Configuration("config/config_resolution.json");
            tik::Log::InitCoreLogger(false, false);
            fonttik.init(&config);
			colorblindFilters = new ColorblindFilters(&config);

            results = colorblindFilters->applyColorblindFilters(cv::imread("config/colorblindness/multi_color_grid.png"));
        }

        void TearDown() override {
            delete colorblindFilters;
        }

        Fonttik fonttik;
        Configuration config;
        ColorblindFilters* colorblindFilters;
        std::vector<cv::Mat> results;
    };

    TEST_F(ColorblindnessTests, Protanopia) {
        cv::Mat result = results[0];
        cv::Mat expected = cv::imread("config/colorblindness/dalton_protanopia.png");
        ASSERT_EQ(result.size(), expected.size());
        cv::Mat diff;
        cv::absdiff(result, expected, diff);
        double maxDiff = cv::norm(diff, cv::NORM_INF);
        double meanDiff = cv::mean(diff)[0];
        std::cout << "Max difference for Protanopia: " << maxDiff << std::endl;
        std::cout << "Mean difference for Protanopia: " << meanDiff << std::endl;
        ASSERT_LE(maxDiff, 5.0);
    }

    TEST_F(ColorblindnessTests, Deuteranopia) {
        cv::Mat result = results[1];
        cv::Mat expected = cv::imread("config/colorblindness/dalton_deuteranopia.png");
        ASSERT_EQ(result.size(), expected.size());
        cv::Mat diff;
        cv::absdiff(result, expected, diff);
        double maxDiff = cv::norm(diff, cv::NORM_INF);
        double meanDiff = cv::mean(diff)[0];
        std::cout << "Max difference for Deuteranopia: " << maxDiff << std::endl;
        std::cout << "Mean difference for Deuteranopia: " << meanDiff << std::endl;
        ASSERT_LE(maxDiff, 5.0);
    }
    
    TEST_F(ColorblindnessTests, Tritanopia) {
        cv::Mat result = results[2];
        cv::Mat expected = cv::imread("config/colorblindness/dalton_tritanopia.png");
        ASSERT_EQ(result.size(), expected.size());
        cv::Mat diff;
        cv::absdiff(result, expected, diff);
        double maxDiff = cv::norm(diff, cv::NORM_INF);
        double meanDiff = cv::mean(diff)[0];
        std::cout << "Max difference for Tritanopia: " << maxDiff << std::endl;
        std::cout << "Mean difference for Tritanopia: " << meanDiff << std::endl;
        ASSERT_LE(maxDiff, 5.0);
    }

	TEST_F(ColorblindnessTests, Grayscale) {
        cv::Mat result = results[3];
        cv::Mat expected = cv::imread("config/colorblindness/grayscale.png");
        ASSERT_EQ(result.size(), expected.size());
        cv::Mat diff;
        cv::absdiff(result, expected, diff);
        double maxDiff = cv::norm(diff, cv::NORM_INF);
        double meanDiff = cv::mean(diff)[0];
        std::cout << "Max difference for Grayscale: " << maxDiff << std::endl;
		std::cout << "Mean difference for Grayscale: " << meanDiff << std::endl;
        ASSERT_LE(maxDiff, 5.0);
    }
}