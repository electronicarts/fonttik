#include <gtest/gtest.h>
#include "Configuration.h"
#include "Guideline.h"

namespace tin {

	TEST(GuidelineTests, 720_legal_guidelines) {
		Configuration cfg("config.json");
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveResolution(720);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 19);
	}

	TEST(GuidelineTests, 1080_legal_guidelines) {
		Configuration cfg("config.json");
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveResolution(1080);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}

	TEST(GuidelineTests, 2160_legal_guidelines) {
		Configuration cfg("config.json");
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveResolution(2160);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 14);
		ASSERT_EQ(guideline->getHeightRequirement(), 92);
	}

	TEST(GuidelineTests, missing_resolution) {
		Configuration cfg("config.json");
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveResolution(1);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}

	TEST(GuidelineTests, missing_configuration_file) {
		Configuration cfg("non-existent_file");
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveResolution(1080);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}

	TEST(GuidelineTests, malformed_configuration) {
		Configuration cfg("unit_test/config_malformed.json");
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveResolution(1080);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}
}
