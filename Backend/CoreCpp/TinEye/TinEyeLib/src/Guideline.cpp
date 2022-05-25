#include "Guideline.h"

namespace tin {
	void Guideline::init(nlohmann::json guidelineJson) {
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

		dpiGuidelines = std::unordered_map<int, SizeGuidelines>();
		for (auto it = guidelineJson["dpi"].begin(); it != guidelineJson["dpi"].end(); ++it)
		{
			SizeGuidelines a(it.value()["width"], it.value()["height"]);
			dpiGuidelines[atoi(it.key().c_str())] = a;
		}

		contrastRatio = guidelineJson["contrast"];
		contrastRatioRecommendation = guidelineJson["recommendedContrast"];
		textBackgroundRadius = guidelineJson["textBackgroundRadius"];
	}

	void Guideline::setActiveGuideline(int resolution) {

		std::unordered_map<int,SizeGuidelines> *sizesInUse = (usingDPI) ? &dpiGuidelines : &resolutionGuidelines;

		auto foundRes = sizesInUse->find(resolution);
		//Check if specified resolution was added during file load
		if (foundRes == sizesInUse->end()) {
			//If not found, error and create default for 1080
			BOOST_LOG_TRIVIAL(error) << "Specified resolution not found, using 1080p, 28x4px as baseline" << std::endl;
			if (!usingDPI) {
				resolutionGuidelines[1080] = SizeGuidelines(4, 28);
				activeGuideline = &resolutionGuidelines.find(1080)->second;
			}
			else {
				dpiGuidelines[100] = SizeGuidelines(0, 18);
				activeGuideline = &dpiGuidelines.find(100)->second;
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
