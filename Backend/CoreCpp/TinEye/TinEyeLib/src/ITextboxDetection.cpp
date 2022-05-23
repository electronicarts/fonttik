#include "ITextboxDetection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <boost/log/trivial.hpp>
#include "AppSettings.h"
#include "TextDetectionParams.h"
#include "Instrumentor.h"

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
				if (boxIt != targetIt) {
					auto overlap = Textbox::OverlapAxisPercentage(*boxIt, *targetIt);
					if (overlap.first >= mergeThreshold.first && overlap.second >= mergeThreshold.second) {
						BOOST_LOG_TRIVIAL(info) << boxIt->getRect() << "merges with " << targetIt->getRect() << std::endl;
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