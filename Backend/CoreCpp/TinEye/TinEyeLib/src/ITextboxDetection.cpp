//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "ITextboxDetection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "AppSettings.h"
#include "TextDetectionParams.h"
#include "Instrumentor.h"
#include "Log.h"

namespace tin {
	
	float ITextboxDetection::HorizontalTiltAngle(const cv::Point& a, const cv::Point& b) {
		double hip = cv::norm(b - a);
		double h = std::abs(a.y - b.y);
		return asin(h / hip);
	}
	void ITextboxDetection::mergeTextBoxes(std::vector<Textbox>& boxes, const TextDetectionParams* params) {
		PROFILE_FUNCTION();
		std::pair<float, float> mergeThreshold = params->getMergeThreshold();

		for (auto boxIt = boxes.begin(); boxIt != boxes.end(); boxIt++) {
			for (auto targetIt = boxIt; targetIt != boxes.end(); ) {
				//If two boxes overlap over our thresholds, we merge them
				if (boxIt != targetIt) {
					auto overlap = Textbox::OverlapAxisPercentage(*boxIt, *targetIt);
					if (overlap.first >= mergeThreshold.first && overlap.second >= mergeThreshold.second) {
						LOG_CORE_INFO("{0} merges with {1}", boxIt->getRect(), targetIt->getRect());
						boxIt->mergeWith(*targetIt);
						targetIt = boxes.erase(targetIt);
					}
					else {
						targetIt++;
					}
				}
				else {
					targetIt++;
				}
			}
		}

	}

}