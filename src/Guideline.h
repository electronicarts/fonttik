//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <map>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>


namespace fs = std::filesystem;

namespace tik {

struct SizeGuidelines {
	size_t width;
	size_t height;
	SizeGuidelines() :width(0), height(0) {}
	SizeGuidelines(size_t w, size_t h)
		:width(w), height(h) {}
};

class Guideline {
	
public:
	Guideline() : contrastRatio(4.5), contrastRatioRecommendation(4.5),
		textBackgroundRadius(10), resolutionGuidelines({ {1080,{4,28}} }) {
		textSizeRatio[0] = 1; textSizeRatio[1] = 3; textSizeRatio[2] = 1;
	}

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
	float* getTextSizeRatio() { return textSizeRatio; }

private:
	//Guidelines
	float contrastRatio;
	float contrastRatioRecommendation;
	float textSizeRatio[3];
	int textBackgroundRadius;
	int heightPer100DPI;
	std::unordered_map<int, SizeGuidelines> resolutionGuidelines;
	std::unordered_map<int, SizeGuidelines> resolutionRecommendations;
	std::unordered_map<int, SizeGuidelines> dpiGuidelines;
	SizeGuidelines* activeGuideline = nullptr;
	SizeGuidelines* activeRecommendation = nullptr;
	bool usingDPI = false;
};

}