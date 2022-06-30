//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "Textbox.h"

namespace tin {

	Textbox::Textbox(const std::vector<cv::Point >& points, int padding) : parentImage(nullptr) {
		//Height takes into account possible box skewing when calculating due to letters going down (eg p's)
		cv::Point topLeft(std::min(points[0].x, points[1].x), points[1].y);
		//Top left height is directly points[1] because through our testing we've found EAST tends to consistently
		//place points[2] a bit higher and that messes up measurement accuracy
		cv::Point bottomRight(std::max(points[2].x, points[3].x), std::max(points[0].y, points[3].y));
		int boxHeight = bottomRight.y - topLeft.y;
		int boxWidth = bottomRight.x - topLeft.x;

		textboxRect = cv::Rect(topLeft.x, topLeft.y, boxWidth, boxHeight);
	}

	Textbox::Textbox(cv::Rect rect) :parentImage(nullptr) {
		textboxRect = rect;
	}

	void Textbox::setParentMedia(Frame* frame) {
		parentImage = frame;
		submatrix = frame->getImageMatrix()(textboxRect);
	}

	cv::Mat Textbox::getLuminanceHistogram() {
		if (luminanceHistogram.empty()) {
			luminanceHistogram = parentImage->calculateLuminanceHistogram(textboxRect);
		}

		return luminanceHistogram;
	}

	cv::Mat Textbox::getSurroundingLuminanceHistogram(int marginX, int marginY)
	{
		//Make sure the surrounding bounding box is inside the original matrix's bounds
		cv::Mat mat = parentImage->getImageMatrix();
		int
			marginLeft = (textboxRect.x - marginX >= 0) ? marginX : textboxRect.x,
			marginRight = (textboxRect.x + textboxRect.width + marginX < mat.cols) ? marginX : mat.cols - textboxRect.width - textboxRect.x,
			marginTop = (textboxRect.y - marginY >= 0) ? marginY : textboxRect.y,
			marginBottom = (textboxRect.y + textboxRect.height + marginY < mat.rows) ? marginY : mat.rows - textboxRect.height - textboxRect.y;
		return parentImage->calculateLuminanceHistogram(cv::Rect(textboxRect.x - marginLeft, textboxRect.y - marginTop,
			textboxRect.width + marginLeft + marginRight, textboxRect.height + marginTop + marginBottom),
			cv::Rect(marginLeft, marginRight, textboxRect.width, textboxRect.height));
	}

	std::pair<float, float> Textbox::OverlapAxisPercentage(const Textbox& a, const Textbox& b) {
		cv::Rect aRect = a.textboxRect, bRect = b.textboxRect;
		cv::Rect overlap = aRect & bRect;
		float xOverlap = static_cast<float>(overlap.width) / std::min(aRect.width, bRect.width);
		float yOverlap = static_cast<float>(overlap.height) / std::min(aRect.height, bRect.height);

		return { xOverlap,yOverlap };
	}

	cv::Mat Textbox::getTextMask() {
		if (textMask.empty()) {
			textMask = calculateTextMask();
		}
		return textMask;
	}

	cv::Mat Textbox::calculateTextMask() {
		//OTSU threshold automatically calculates best fitting threshold values
		cv::Mat unsignedLuminance, mask;
		cv::Mat luminanceRegion = getLuminanceMap();
		luminanceRegion.convertTo(unsignedLuminance, CV_8UC1, 255);
		cv::threshold(unsignedLuminance, mask, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);

		/*
		Ensure that text is being highlighted
		If text mask has an 'outline' of the box on top and bottom sides it means that most probably is inverted.
		Another possible approach is to count masked pixels and non-masked pixels but for wider or bolder fonts
		it doesn't work.
		*/

		cv::Mat topSide = mask(cv::Range(0, 1), cv::Range::all()),
			botSide = mask(cv::Range(mask.rows - 1, mask.rows), cv::Range::all());

		//If there's more positive pixels in top and bottom row than pixels in a single row, invert the mask
		if (cv::countNonZero(topSide) + cv::countNonZero(botSide) - mask.cols > 0) {
			cv::bitwise_not(mask, mask);
		}

		return mask;
	}
}
