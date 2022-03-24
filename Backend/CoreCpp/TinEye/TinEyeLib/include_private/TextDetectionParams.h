#pragma once
#include <array>
#include <string>

namespace tin {
	class TextDetectionParams {
	private:
		std::string detectionModel;
		float confThreshold; //Confidence threshold
		float nmsThreshold; //Non maximum supresison threshold
		double detScale; //Scaes pixel individually after mean substraction
		std::array<double, 3> detMean;//This values will be substracted from the corresponding channel
		std::pair<float, float> mergeThreshold; //If overlap in both axes surpasses this value, textboxes will be merged
		float rotationThresholdRadians; //Text that excedes this inclination will be ignored (not part of the HUD)

	public:
		TextDetectionParams() {}
		TextDetectionParams(std::string model,float confidenceThreshold, float NMSThreshold,
			double detectionScale, std::array<double, 3> detectionMean, std::pair<float, float> mergeThreshold,
			float rotationThresholdRadians) :
			detectionModel(model),
			confThreshold(confidenceThreshold), nmsThreshold(NMSThreshold),
			detScale(detectionScale), detMean(detectionMean), mergeThreshold(mergeThreshold),
			rotationThresholdRadians(rotationThresholdRadians) {};

		std::string getDetectionModel() const { return detectionModel; }
		float getConfidenceThreshold() const { return confThreshold; }
		float getNMSThreshold() const { return nmsThreshold; }
		float getRotationThresholdRadians() const { return rotationThresholdRadians; }
		double getDetectionScale() const { return detScale; }
		std::array<double, 3> getDetectionMean() const { return detMean; }
		std::pair<float, float> getMergeThreshold() const { return mergeThreshold; }
	};
}