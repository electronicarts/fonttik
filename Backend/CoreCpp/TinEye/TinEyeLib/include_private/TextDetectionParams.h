#pragma once
#include <array>

namespace tin {
	class TextDetectionParams {
	private:
		float confThreshold; //Confidence threshold
		float nmsThreshold; //Non maximum supresison threshold
		double detScale; //Scaes pixel individually after mean substraction
		std::array<double, 3> detMean;//This values will be substracted from the corresponding channel
		std::pair<float, float> mergeThreshold; //If overlap in both axes surpasses this value, textboxes will be merged
		float rotationThresholdRadians; //Text that excedes this inclination will be ignored (not part of the HUD)

	public:
		TextDetectionParams() {}
		TextDetectionParams(float confidenceThreshold, float NMSThreshold,
			double detectionScale, std::array<double, 3> detectionMean, std::pair<float, float> mergeThreshold,
			float rotationThresholdRadians) :
			confThreshold(confidenceThreshold), nmsThreshold(NMSThreshold),
			detScale(detectionScale), detMean(detectionMean), mergeThreshold(mergeThreshold),
			rotationThresholdRadians(rotationThresholdRadians) {};

		float getConfidenceThreshold() const { return confThreshold; }
		float getNMSThreshold() const { return nmsThreshold; }
		float getRotationThresholdRadians() const { return rotationThresholdRadians; }
		double getDetectionScale() const { return detScale; }
		std::array<double, 3> getDetectionMean() const { return detMean; }
		std::pair<float, float> getMergeThreshold() const { return mergeThreshold; }
	};
}