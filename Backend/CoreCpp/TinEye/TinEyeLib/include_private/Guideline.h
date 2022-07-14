//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <map>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>


namespace fs = std::filesystem;

namespace tin {

	struct SizeGuidelines {
		size_t width;
		size_t height;
		SizeGuidelines() :width(0), height(0) {}
		SizeGuidelines(size_t w, size_t h)
			:width(w), height(h) {}
	};

	class Guideline {
		//Guidelines
		float contrastRatio;
		float contrastRatioRecommendation;
		int textBackgroundRadius;
		std::unordered_map<int, SizeGuidelines> resolutionGuidelines;
		std::unordered_map<int, SizeGuidelines> resolutionRecommendations;
		std::unordered_map<int, SizeGuidelines> dpiGuidelines;
		SizeGuidelines* activeGuideline = nullptr;
		SizeGuidelines* activeRecommendation = nullptr;
		bool usingDPI = false;
		int heightPer100DPI;
	public:
		Guideline() : contrastRatio(4.5), contrastRatioRecommendation(4.5),
		textBackgroundRadius(10), resolutionGuidelines({ {1080,{4,28}} }) {}

		void init(nlohmann::json guidelineJson);

		void setActiveGuideline(int guideline);
		void setDPI(bool to) { usingDPI = to; }

		size_t getHeightRequirement() const { return activeGuideline->height; }
		size_t getWidthRequirement() const { return activeGuideline->width; }
		float getContrastRequirement() const { return contrastRatio; }

		size_t getHeightRecommendation() const { return activeRecommendation->height; }
		size_t getWidthRecommendation() const { return activeRecommendation->width; }
		float getContrastRecommendation() const { return contrastRatioRecommendation; }

		int getTextBackgroundRadius() { return textBackgroundRadius; }
	};

}