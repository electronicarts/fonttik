#include <gtest/gtest.h>
#include "Configuration.h"

TEST(ConfigurationTests, 720_legal_guidelines) {
	Configuration cfg("config.json");
	cfg.setActiveResolution(720);
	ASSERT_EQ(cfg.getContrastRequirement(), 4.5);
	ASSERT_EQ(cfg.getWidthRequirement(), 4);
	ASSERT_EQ(cfg.getHeightRequirement(), 19);
}

TEST(ConfigurationTests,1080_legal_guidelines) {
	Configuration cfg("config.json");
	cfg.setActiveResolution(1080);
	ASSERT_EQ(cfg.getContrastRequirement(), 4.5);
	ASSERT_EQ(cfg.getWidthRequirement(), 4);
	ASSERT_EQ(cfg.getHeightRequirement(), 28);
}

TEST(ConfigurationTests, 2160_legal_guidelines) {
	Configuration cfg("config.json");
	cfg.setActiveResolution(2160);
	ASSERT_EQ(cfg.getContrastRequirement(), 4.5);
	ASSERT_EQ(cfg.getWidthRequirement(), 14);
	ASSERT_EQ(cfg.getHeightRequirement(), 92);
}