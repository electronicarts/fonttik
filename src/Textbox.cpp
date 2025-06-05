//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/TextBox.hpp"

namespace tik
{
	TextBox::TextBox(const std::vector<cv::Point>& points, cv::Mat frameImg)
		/*widthSimilarityThreshold(0.45f), heightSimilarityThreshold(0.7f)*/
	{
		//Height takes into account possible box skewing when calculating due to letters going down (eg p's)
		cv::Point topLeft{
			std::min({ points[0].x, points[1].x, points[2].x, points[3].x }),
			std::min({ points[0].y, points[1].y, points[2].y, points[3].y })
		};
		cv::Point bottomRight{
			std::max({ points[0].x, points[1].x, points[2].x, points[3].x }),
			std::max({ points[0].y, points[1].y, points[2].y, points[3].y })
		};
		int boxHeight = bottomRight.y - topLeft.y;
		int boxWidth = bottomRight.x - topLeft.x;

		textBoxRect = cv::Rect(topLeft.x, topLeft.y, boxWidth, boxHeight);
		textRect = textBoxRect;
		textSubMat = frameImg(textBoxRect);
	}

	TextBox::TextBox(cv::Rect rect, cv::Mat frameImg) : textBoxRect(rect)
	{
		textRect = textBoxRect;
		textSubMat = frameImg(textBoxRect);
	}

	void TextBox::calculateTextBoxLuminance(const std::vector<double>& sRgbValues)
	{
		//Linearize BGR to sBGR 
		cv::Mat sBgr(textSubMat.size(), CV_64FC3);
		textMatLuminance = cv::Mat(textSubMat.size(), CV_64FC1);
		cv::LUT(textSubMat, sRgbValues, sBgr);
		sBgr.forEach<cv::Vec3d>(ConvertToRelativeLuminance(&textMatLuminance));
	}
	void TextBox::calculateTextMask()
	{
		//OTSU threshold automatically calculates best fitting threshold values
		cv::Mat unsignedLuminance;
		textMatLuminance.convertTo(unsignedLuminance, CV_8UC1, 255);

		cv::threshold(unsignedLuminance, textMask, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);

		/* Ensure that text is being highlighted
			If text mask has an 'outline' of the box on top and bottom sides it means that most probably is inverted.
			Another possible approach is to count masked pixels and non-masked pixels but for wider or bolder fonts
			it doesn't work. */

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(textMask, contours, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
		
		if (contours.size() < 5) //check if the background is being detected and reverse the mask
		{
			cv::Mat topSide = textMask(cv::Range(0, 1), cv::Range::all()),
				botSide = textMask(cv::Range(textMask.rows - 1, textMask.rows), cv::Range::all());

			//If there's more positive pixels in top and bottom row than pixels in a single row, invert the textMask
			if (cv::countNonZero(topSide) + cv::countNonZero(botSide) - textMask.cols > 0)
			{
				cv::bitwise_not(textMask, textMask);
				//set contours with flipped textMask
				cv::findContours(textMask, contours, cv::RetrievalModes::RETR_EXTERNAL, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE);
			}
		}

		struct RectComparer
		{
			bool operator()(const cv::Rect& rect1, const cv::Rect& rect2) const {
				return (rect1.x + rect1.y) < (rect2.x + rect2.y); //order right to left, top to bottom
			}
		};

		std::vector<cv::Rect> boundingRects;
		boundingRects.reserve(contours.size());

		for (auto& contour : contours)
		{
			cv::Rect rect = cv::boundingRect(contour);
			auto area = rect.area();
			if ( area >= 20 ) //discard small area contours
			{
				boundingRects.emplace_back(rect);
			}
		}

		std::vector<std::vector<cv::Rect>> groupedRects = { {boundingRects[0]} };

		auto areBoxesAligned = [](cv::Rect a, cv::Rect b) -> bool {
			float diffTop = std::abs(a.y - b.y);
			float diffBottom = std::abs((a.y + a.height) - (b.y + b.height));
			float diffTotal = std::abs(diffTop - diffBottom);

			float threshold = std::max(a.height, b.height) / 3.5f;
			

			float diffLeft = std::abs(a.x - b.x);
			float diffRight = std::abs((a.x + a.width) - (b.x + b.width));
			float horizontalDiffTotal = std::abs(diffLeft - diffRight);

			float horizontalThreshold = std::max(a.width, b.width) / 3.5f;


			return diffTop < threshold || diffBottom < threshold || diffTotal < threshold ||
				   diffLeft < horizontalThreshold || diffRight < horizontalThreshold || horizontalDiffTotal;
		} ;


		//group bounding boxes by similar sizes
		for (int i = 1; i < boundingRects.size(); i++)
		{
			bool grouped = false;
			for (auto& group : groupedRects)
			{
				if (areBoxesAligned(boundingRects[i], group[0])/* && isRectWithinBounds(boundingRects[i], groupedRects[group][0])*/)
				{
					group.emplace_back(boundingRects[i]);
					grouped = true;
					break;
				}
			}

			if (!grouped)
			{
				groupedRects.push_back({ boundingRects[i] });
			}
		}

		//it is assumed there is only one group of characters, so there is only one aligned group
		if (groupedRects.size() != 1)
		{
			//sort contour groups by group size
			std::sort(groupedRects.begin(), groupedRects.end(), [](const auto& a, const auto& b)
				{
					return a.size() > b.size();
				});
		}

		std::vector<cv::Point> points;
		points.reserve(groupedRects[0].size() * 2);

		for (auto& rect : groupedRects[0])
		{
			points.emplace_back(rect.tl()); //x,y
			points.emplace_back(rect.br()); //x + w, y + h
		}

		textRect = cv::boundingRect(points);
		//set rect to the bounds of the image
		textRect = textRect & cv::Rect(0, 0, textMask.cols, textMask.rows);

		//set the rest of the mask to 0
		cv::Mat textRectMask = cv::Mat::zeros(textMask.size(), CV_8UC1);
		textRectMask(textRect) = 255;
		textMask = textRectMask & textMask;

		cv::Mat maskPoints;
		cv::findNonZero(textMask, maskPoints);
		textRect = cv::boundingRect(maskPoints);
	}

	std::pair<float, float> TextBox::OverlapAxisPercentage(const TextBox& a, const TextBox& b)
	{
		cv::Rect aRect = a.textBoxRect, bRect = b.textBoxRect;
		cv::Rect overlap = aRect & bRect;
		float xOverlap = static_cast<float>(overlap.width) / std::min(aRect.width, bRect.width);
		float yOverlap = static_cast<float>(overlap.height) / std::min(aRect.height, bRect.height);

		return { xOverlap,yOverlap };
	}

}