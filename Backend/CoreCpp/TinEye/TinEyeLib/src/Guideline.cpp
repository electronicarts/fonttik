#include "Guideline.h"

namespace tin {
	Guideline::Guideline(float contrast, std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines) :
		contrastRatio(contrast),
		resolutionGuidelines(resolutionGuidelines) {}

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
	};
}
