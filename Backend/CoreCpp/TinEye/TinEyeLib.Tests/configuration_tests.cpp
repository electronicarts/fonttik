//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Configuration.h"
#include "Guideline.h"

namespace tin {

	class ResolutionConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			cfg = Configuration("unit_test/config_resolution.json");
		}
	};

	class DPIConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			cfg = Configuration("unit_test/config_dpi.json");
		}
	};

	class MalformedConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			cfg = Configuration("unit_test/config_malformed.json");
		}
	};

	class MissingConfiguration : public ::testing::Test {
	protected:
		Configuration cfg;

		void SetUp() override {
			cfg = Configuration("non-existent-file");
		}
	};

	TEST_F(ResolutionConfiguration, 720_legal_guidelines) {
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveGuideline(720);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 19);
	}

	TEST_F(ResolutionConfiguration, 1080_legal_guidelines) {
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveGuideline(1080);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}

	TEST_F(ResolutionConfiguration, 2160_legal_guidelines) {
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveGuideline(2160);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 10);
		ASSERT_EQ(guideline->getHeightRequirement(), 52);
	}

	TEST_F(ResolutionConfiguration, missing_resolution) {
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveGuideline(1);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}

	TEST_F(DPIConfiguration, correct_dpi_calc) {
		Guideline* guideline = cfg.getGuideline();
		guideline->setDPI(cfg.getAppSettings()->usingDPI());
		int dpis[5] = { 100,200,400,636,570 };
		for (const auto& dpi : dpis) {
			guideline->setActiveGuideline(dpi);
			ASSERT_EQ(guideline->getWidthRequirement(), 0);
			//Hardcoded 18pxper100m to follow microsoft XAG v3 (2022)
			ASSERT_EQ(guideline->getHeightRequirement(), static_cast<int>((dpi / 100.0) * 18));
		}
	}

	//Builds with hard-coded values and doesnt throw exceptions
	TEST_F(MissingConfiguration, default_guideline) {
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveGuideline(1080);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}

	//Builds with hard-coded values and doesnt throw exceptions
	TEST_F(MissingConfiguration, default_detection) {
		TextDetectionParams* params = cfg.getTextDetectionParams();
		ASSERT_FLOAT_EQ(0.5, params->getConfidenceThreshold());
		ASSERT_FLOAT_EQ(0.4, params->getNMSThreshold());
		ASSERT_FLOAT_EQ(1.0, params->getDetectionScale());
		ASSERT_THAT(params->getDetectionMean(), ::testing::ElementsAre(123.68, 116.78, 103.94));
		ASSERT_FLOAT_EQ(params->getMergeThreshold().first, 1.0);
		ASSERT_FLOAT_EQ(params->getMergeThreshold().second, 1.0);
		ASSERT_FLOAT_EQ(0.17, params->getRotationThresholdRadians());
	}

	//Builds with hard-coded values and doesnt throw exceptions
	TEST_F(MissingConfiguration, default_recognition) {
		TextRecognitionParams* params = cfg.getTextRecognitionParams();
		ASSERT_EQ("crnn_cs.onnx", params->getRecognitionModel());
		ASSERT_EQ("CTC-greedy", params->getDecodeType());
		ASSERT_EQ("alphabet_94.txt", params->getVocabularyFilepath());
		ASSERT_DOUBLE_EQ(1.0 / 127.5, params->getScale());
		ASSERT_THAT(params->getMean(), ::testing::ElementsAre(127.5, 127.5, 127.5));
		ASSERT_EQ(100, params->getSize().first);
		ASSERT_EQ(32, params->getSize().second);
	}

	TEST_F(MalformedConfiguration, default_guideline) {
		Guideline* guideline = cfg.getGuideline();
		guideline->setActiveGuideline(1080);
		ASSERT_EQ(guideline->getContrastRequirement(), 4.5);
		ASSERT_EQ(guideline->getWidthRequirement(), 4);
		ASSERT_EQ(guideline->getHeightRequirement(), 28);
	}

	TEST_F(MalformedConfiguration, default_detection) {
		TextDetectionParams* params = cfg.getTextDetectionParams();
		ASSERT_FLOAT_EQ(0.5, params->getConfidenceThreshold());
		ASSERT_FLOAT_EQ(0.4, params->getNMSThreshold());
		ASSERT_FLOAT_EQ(1.0, params->getDetectionScale());
		ASSERT_THAT(params->getDetectionMean(), ::testing::ElementsAre(123.68, 116.78, 103.94));
		ASSERT_FLOAT_EQ(params->getMergeThreshold().first, 1.0);
		ASSERT_FLOAT_EQ(params->getMergeThreshold().second, 1.0);
		ASSERT_FLOAT_EQ(0.17, params->getRotationThresholdRadians());
	}

	TEST_F(MalformedConfiguration, default_recognition) {
		TextRecognitionParams* params = cfg.getTextRecognitionParams();
		ASSERT_EQ("crnn_cs.onnx", params->getRecognitionModel());
		ASSERT_EQ("CTC-greedy", params->getDecodeType());
		ASSERT_EQ("alphabet_94.txt", params->getVocabularyFilepath());
		ASSERT_DOUBLE_EQ(1.0 / 127.5, params->getScale());
		ASSERT_THAT(params->getMean(), ::testing::ElementsAre(127.5, 127.5, 127.5));
		ASSERT_EQ(100, params->getSize().first);
		ASSERT_EQ(32, params->getSize().second);
	}
}
