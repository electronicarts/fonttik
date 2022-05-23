#pragma once
#include <array>
#include <string>
#include <nlohmann/json.hpp>

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
		TextDetectionParams() : detectionModel("frozen_east_text_detection.pb"),
			confThreshold(0.5), nmsThreshold(0.4), detScale(1.0),
			detMean({ 123.68, 116.78, 103.94 }), mergeThreshold({ 1.0,1.0 }),
			rotationThresholdRadians(0.17) {}

		void init(nlohmann::json textDetection) {
			nlohmann::json mean = textDetection["detectionMean"];
			nlohmann::json merge = textDetection["mergeThreshold"];
			float degreeThreshold = textDetection["rotationThresholdDegrees"];

			detectionModel = textDetection["detectionModel"];
			confThreshold = textDetection["confidence"];
			nmsThreshold = textDetection["nmsThreshold"];
			detScale = textDetection["detectionScale"];
			detMean = { mean[0],mean[1] ,mean[2] };
			mergeThreshold = std::make_pair(merge["x"], merge["y"]);
			rotationThresholdRadians = degreeThreshold * (CV_PI / 180);
		}

		std::string getDetectionModel() const { return detectionModel; }
		float getConfidenceThreshold() const { return confThreshold; }
		float getNMSThreshold() const { return nmsThreshold; }
		float getRotationThresholdRadians() const { return rotationThresholdRadians; }
		double getDetectionScale() const { return detScale; }
		std::array<double, 3> getDetectionMean() const { return detMean; }
		std::pair<float, float> getMergeThreshold() const { return mergeThreshold; }
		void setMergeThreshold(std::pair<float, float> threshold) { mergeThreshold = threshold; }
	};
}