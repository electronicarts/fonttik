#pragma once
#include <map>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>


namespace fs = std::filesystem;

namespace tin {

	struct ResolutionGuidelines {
		size_t width;
		size_t height;
		ResolutionGuidelines() :width(0), height(0) {}
		ResolutionGuidelines(size_t w, size_t h)
			:width(w), height(h) {}
	};

	class Guideline {
		//Guidelines
		float contrastRatio;
		float contrastRatioRecommendation;
		int textBackgroundRadius;
		std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines;
		std::unordered_map<int, ResolutionGuidelines> resolutionRecommendations;
		ResolutionGuidelines* activeResolution = nullptr;
		ResolutionGuidelines* activeRecommendation = nullptr;
	public:
		Guideline() : contrastRatio(4.5), contrastRatioRecommendation(4.5),
		textBackgroundRadius(10), resolutionGuidelines({ {1080,{4,28}} }) {}

		void init(nlohmann::json guidelineJson);

		void setActiveResolution(int resolution);

		size_t getHeightRequirement() const { return activeResolution->height; }
		size_t getWidthRequirement() const { return activeResolution->width; }
		float getContrastRequirement() const { return contrastRatio; }

		size_t getHeightRecommendation() const { return activeRecommendation->height; }
		size_t getWidthRecommendation() const { return activeRecommendation->width; }
		float getContrastRecommendation() const { return contrastRatioRecommendation; }

		int getTextBackgroundRadius() { return textBackgroundRadius; }
	};

}