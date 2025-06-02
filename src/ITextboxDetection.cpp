//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "ITextboxDetection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "fonttik/ConfigurationParams.hpp"
#include "fonttik/Log.h"
#include "fonttik/TextBox.hpp"

namespace tik {
	
	float ITextboxDetection::HorizontalTiltAngle(const cv::Point& a, const cv::Point& b) 
	{
		double hip = cv::norm(b - a);
		double h = std::abs(a.y - b.y);
		return asin(h / hip);
	}

	void ITextboxDetection::mergeTextBoxes(std::vector<TextBox>& boxes, cv::Mat img) 
	{
		std::pair<float, float> mergeThreshold = detectionParams->mergeThreshold;

		for (auto boxIt = boxes.begin(); boxIt != boxes.end(); boxIt++) 
		{
			for (auto targetIt = boxIt; targetIt != boxes.end(); ) 
			{
				//If two boxes overlap over our thresholds, we merge them
				if (boxIt != targetIt) {
					auto overlap = TextBox::OverlapAxisPercentage(*boxIt, *targetIt);
					if (overlap.first >= mergeThreshold.first && overlap.second >= mergeThreshold.second) 
					{
						LOG_CORE_TRACE("{0} merges with {1}", boxIt->getTextBoxRect(), targetIt->getTextBoxRect());
						boxIt->mergeWith(*targetIt,img);
						targetIt = boxes.erase(targetIt);
					}
					else 
					{
						targetIt++;
					}
				}
				else 
				{
					targetIt++;
				}
			}
		}

	}

}