#pragma once
#include <map>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <boost/log/trivial.hpp>


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
		int textBackgroundRadius;
		std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines;
		ResolutionGuidelines* activeResolution = nullptr;
	public:
		Guideline() {}
		Guideline(float contrast, int textRadius, std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines);

		void setActiveResolution(int resolution);

		size_t getHeightRequirement() const { return activeResolution->height; }
		size_t getWidthRequirement() const { return activeResolution->width; }
		float getContrastRequirement() const { return contrastRatio; }
		int getTextBackgroundRadius() { return textBackgroundRadius; }
	};

}