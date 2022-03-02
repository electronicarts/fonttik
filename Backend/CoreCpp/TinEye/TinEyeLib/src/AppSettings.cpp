#include "AppSettings.h"
#include <tuple>
#include <opencv2/imgcodecs.hpp>

void AppSettings::setFocusMask(std::vector<cv::Rect2f> focus, std::vector<cv::Rect2f> ignore) {
	if (!focus.empty()) {
		focusMasks = focus;
	}
	else {
		focusMasks = { {0,0,1,1} }; //If theres no focus region, we will analyse everything
	}

	ignoreMasks = ignore;
}

cv::Mat AppSettings::calculateMask(int width, int height) {
	cv::Mat mat(height, width, CV_8UC3, cv::Scalar(0,0,0));
	
	for (cv::Rect2f rect : focusMasks) {
		cv::Rect absRect(rect.x * width, rect.y * height,
			rect.width * width, rect.height * height);
		
		cv::Mat subMatrix = mat(absRect);
		subMatrix.setTo(cv::Scalar(255,255,255));
	}

	//Ignore masks will be ignored even if inside focus regions
	for (cv::Rect2f rect : ignoreMasks) {
		cv::Rect absRect(rect.x * width, rect.y * height,
			rect.width * width, rect.height * height);

		cv::Mat subMatrix = mat(absRect);
		subMatrix.setTo(cv::Scalar(0, 0, 0));
	}
	return mat;
}