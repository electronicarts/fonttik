#pragma once
#include "Frame.h"
#include <opencv2/core.hpp>


namespace tin {
	class Textbox {
	protected:
		cv::Rect textboxRect;

		cv::Mat submatrix;
		cv::Mat luminanceHistogram;
		cv::Mat textMask;

		Frame* parentImage = nullptr;
		cv::Mat calculateTextMask();
	public:
		/* Operator method
		* Returns the percentage of overlap in each axis between two rectangles in relation to the smallest one
		* @return (x axis overlap, y axis overlap)
		* */
		static std::pair<float, float> OverlapAxisPercentage(const Textbox& a, const Textbox& b);
		bool operator==(const Textbox& b) const{
			return textboxRect == b.textboxRect && parentImage == b.parentImage;
		}

		Textbox(const std::vector<cv::Point >& points, int padding = 0);
		Textbox(cv::Rect rect);
		//Sets textbox's parent image and calculates its submatrix
		void setParentMedia(Frame* media);

		void mergeWith(Textbox& other) { textboxRect = textboxRect | other.getRect(); };

		//Returns and saves the luminance histogram of the specified textbox rect
		cv::Mat getLuminanceHistogram();

		//Returns the luminance histogram of the area surrounding the textbox rect
		cv::Mat getSurroundingLuminanceHistogram(int marginX = 3, int marginY = 3);

		cv::Rect getRect() const { return textboxRect; }
		cv::Mat getSubmatrix() const { return submatrix; }
		cv::Mat getLuminanceMap() const { return parentImage->getFrameLuminance()(textboxRect); };
		cv::Mat getTextMask();
	};
}