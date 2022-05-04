#include "Guideline.h"

namespace tin {
	Guideline::Guideline(float contrast, float contrastRec, int textRadius, std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines) :
		contrastRatio(contrast), textBackgroundRadius(textRadius),
		resolutionGuidelines(resolutionGuidelines),
		contrastRatioRecommendation(contrastRec) {}

	Guideline::Guideline(float contrast, float contrastRec, int textRadius, std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines, std::unordered_map<int, ResolutionGuidelines> resolutionRecs) :
		contrastRatio(contrast), textBackgroundRadius(textRadius),
		resolutionGuidelines(resolutionGuidelines),
		contrastRatioRecommendation(contrastRec), resolutionRecommendations(resolutionRecs) {}

	void Guideline::setActiveResolution(int resolution) {
		auto foundRes = resolutionGuidelines.find(resolution);
		//Check if specified resolution was added during file load
		if (foundRes == resolutionGuidelines.end()) {
			//If not found, error and create default for 1080
			BOOST_LOG_TRIVIAL(error) << "Specified resolution not found, using 1080p, 28x4px as baseline" << std::endl;
			resolutionGuidelines[1080] = ResolutionGuidelines(4, 28);
			activeResolution = &resolutionGuidelines.find(1080)->second;
		}
		else {
			activeResolution = &foundRes->second;
		}

		//See if there are recommended measurements for this resolution, if not then default to required measurements
		auto foundRec = resolutionRecommendations.find(resolution);
		if (foundRec == resolutionRecommendations.end()) {
			activeRecommendation = activeResolution;
		}
		else {
			activeRecommendation = &foundRec->second;
		}
	};
}
