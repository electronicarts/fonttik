//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "fonttik/Configuration.hpp"
#include "fonttik/Log.h"

namespace tik {

	class ResolutionConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			cfg = Configuration("config/config_resolution.json");
		}
	};

	class DPIConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			cfg = Configuration("config/config_dpi.json");
		}
	};

	class MalformedConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			cfg = Configuration("config/config_malformed.json");
		}
	};

	class MissingConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			tik::Log::InitCoreLogger(false, false);
			cfg = Configuration("non-existent-file");
		}
	};

	TEST_F(ResolutionConfiguration, 720_legal_guidelines) {
		cfg.setResolutionGuideline("720");
		ASSERT_EQ(cfg.getContrastRatioParams().contrastRatio, 4.5f);
		ASSERT_EQ(cfg.getTextSizeParams().activeGuideline->width, 4);
		ASSERT_EQ(cfg.getTextSizeParams().activeGuideline->height, 19);
	}

	TEST_F(ResolutionConfiguration, 1080_legal_guidelines) {
		cfg.setResolutionGuideline("1080");
		ASSERT_EQ(cfg.getContrastRatioParams().contrastRatio, 4.5f);
		ASSERT_EQ(cfg.getTextSizeParams().activeGuideline->width, 4);
		ASSERT_EQ(cfg.getTextSizeParams().activeGuideline->height, 28);
	}

	TEST_F(ResolutionConfiguration, 2160_legal_guidelines) {
		cfg.setResolutionGuideline("2160");
		ASSERT_EQ(cfg.getContrastRatioParams().contrastRatio, 4.5f);
		ASSERT_EQ(cfg.getTextSizeParams().activeGuideline->width, 10);
		ASSERT_EQ(cfg.getTextSizeParams().activeGuideline->height, 52);
	}
}
