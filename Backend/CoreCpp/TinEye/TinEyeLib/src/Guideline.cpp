//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "Guideline.h"

namespace tin {
	void Guideline::init(nlohmann::json guidelineJson) {
		//Load resolution guidelines and recommendations with the resolution as key and width & height as values
		resolutionGuidelines = std::unordered_map<int, SizeGuidelines>();
		for (auto it = guidelineJson["resolutions"].begin(); it != guidelineJson["resolutions"].end(); ++it)
		{
			SizeGuidelines a(it.value()["width"], it.value()["height"]);
			resolutionGuidelines[atoi(it.key().c_str())] = a;
		}

		resolutionRecommendations = std::unordered_map<int, SizeGuidelines>();
		for (auto it = guidelineJson["resolutionsRecommendations"].begin(); it != guidelineJson["resolutionsRecommendations"].end(); ++it)
		{
			SizeGuidelines a(it.value()["width"], it.value()["height"]);
			resolutionRecommendations[atoi(it.key().c_str())] = a;
		}
		dpiGuidelines = {};
		heightPer100DPI = guidelineJson["heightPer100DPI"];
		contrastRatio = guidelineJson["contrast"];
		contrastRatioRecommendation = guidelineJson["recommendedContrast"];
		textBackgroundRadius = guidelineJson["textBackgroundRadius"];

		for (int i = 0; i < 3; i++) {
			textSizeRatio[i] = guidelineJson["textSizeRatio"][i];
		}
	}

	void Guideline::setActiveGuideline(int resolution) {

		std::unordered_map<int,SizeGuidelines> *sizesInUse = (usingDPI) ? &dpiGuidelines : &resolutionGuidelines;

		auto foundRes = sizesInUse->find(resolution);
		//Check if specified resolution was added during file load
		if (foundRes == sizesInUse->end()) {
			if (!usingDPI) {
				//If not found, error and create default for 1080
				BOOST_LOG_TRIVIAL(error) << "Specified resolution not found, using 1080p, 28x4px as baseline" << std::endl;
				resolutionGuidelines[1080] = SizeGuidelines(4, 28);
				activeGuideline = &resolutionGuidelines.find(1080)->second;
			}
			else {
				//If using DPI simply calculate linearly based on 100DPI baseline
				dpiGuidelines[resolution] = SizeGuidelines(0, 
					static_cast<int>(heightPer100DPI*(resolution/100.0f)));
				activeGuideline = &dpiGuidelines.find(resolution)->second;
			}
			
		}
		else {
			activeGuideline = &foundRes->second;
		}

		//See if there are recommended measurements for this resolution, if not then default to required measurements
		
		auto foundRec = resolutionRecommendations.find(resolution);
		if (foundRec == resolutionRecommendations.end()) {
			activeRecommendation = activeGuideline;
		}
		else {
			activeRecommendation = &foundRec->second;
		}
	};
}
