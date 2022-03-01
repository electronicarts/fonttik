#pragma once
#include <array>

class TextDetectionParams {
private:
	float confThreshold; //Confidence threshold
	float nmsThreshold; //Non maximum supresison threshold
	double detScale; //Scaes pixel individually after mean substraction
	std::array<double, 3> detMean;//This values will be substracted from the corresponding channel

public:
	TextDetectionParams(float confidenceThreshold, float NMSThreshold,
	double detectionScale, std::array<double, 3> detectionMean) :
		confThreshold(confidenceThreshold), nmsThreshold(NMSThreshold),
		detScale(detectionScale),detMean(detectionMean) {};

	float getConfidenceThreshold() const { return confThreshold; }
	float getNMSThreshold() const { return nmsThreshold; }
	double getDetectionScale() const { return detScale; }
	std::array<double,3> getDetectionMean() const{ return detMean; }
};