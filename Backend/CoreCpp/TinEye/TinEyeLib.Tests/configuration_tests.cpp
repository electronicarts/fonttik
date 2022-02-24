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

TEST(ConfigurationTests, missing_resolution) {
	Configuration cfg("config.json");
	cfg.setActiveResolution(1);
	ASSERT_EQ(cfg.getWidthRequirement(), 4);
	ASSERT_EQ(cfg.getHeightRequirement(), 28);
}

TEST(ConfigurationTests, missing_configuration_file) {
	Configuration cfg("non-existent_file");
	ASSERT_EQ(cfg.getContrastRequirement(), 4.5);
	ASSERT_EQ(cfg.getLanguage(), "eng");
	ASSERT_TRUE(cfg.isValidLanguage());
}

TEST(ConfigurationTests, missing_language) {
	Configuration cfg("config.json");
	cfg.setActiveLanguage("asdf");
	ASSERT_EQ(cfg.getLanguage(), "eng");
	ASSERT_TRUE(cfg.isValidLanguage());
}

TEST(ConfigurationTests, malformed_configuration) {
	Configuration cfg("unit_test/config_malformed.json");
	cfg.setActiveResolution(1080);
	ASSERT_EQ(cfg.getContrastRequirement(), 4.5);
	ASSERT_EQ(cfg.getWidthRequirement(), 4);
	ASSERT_EQ(cfg.getHeightRequirement(), 28);
	ASSERT_EQ(cfg.getLanguage(), "eng");
	ASSERT_TRUE(cfg.isValidLanguage());
}

TEST(ConfigurationTests, wrong_tessdata_location) {
	Configuration cfg("unit_test/config_wrong_tessdata.json");
	ASSERT_FALSE(cfg.isValidLanguage());
}
