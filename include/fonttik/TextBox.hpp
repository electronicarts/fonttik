//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once

#include "fonttik/Frame.hpp"
#include <opencv2/core.hpp>
#include <optional>
#include <string>

namespace tik
{

class TextBox
{
public:
	TextBox(const std::vector<cv::Point>& points, cv::Mat frameImg);
	TextBox(cv::Rect rect, cv::Mat frameImg);

	void calculateTextBoxLuminance(const std::vector<double>& sRgbValues);

	void calculateTextMask();

	const cv::Mat getSubMatrix() const { return textSubMat; }
	const cv::Mat getTextMask() const { return textMask; }
	const cv::Rect getTextRect() const { return textRect; }
	const cv::Rect getTextBoxRect() const { return textBoxRect; }
	const cv::Mat getTextMatLuminance() const { return textMatLuminance; }
	std::string getText() const { return text; }

	void setText(const std::string& text) { this->text = text; }

	/// <summary>
	/// Changes this textbox into one that includes itself and another
	/// </summary>
	/// <param name="other">absorbed textbox</param>
	void mergeWith(TextBox& other, cv::Mat img) { textBoxRect = textBoxRect | other.getTextBoxRect(); textSubMat = img(textBoxRect); };


	/* Operator method
	* Returns the percentage of overlap in each axis between two rectangles in relation to the smallest one
	* @return (x axis overlap, y axis overlap)
	* */
	static std::pair<float, float> OverlapAxisPercentage(const TextBox& a, const TextBox& b);
	
private:

	struct ConvertToRelativeLuminance
	{
		ConvertToRelativeLuminance(cv::Mat* luminanceMat) { luminance = luminanceMat; };
		cv::Mat* luminance;

		void operator()(cv::Vec3d &pixel, const int* position) const
		{
			//Y = 0.0722 * B + 0.7152 * G + 0.2126 * R where B, G and R
			luminance->ptr<double>(position[0])[position[1]] = 0.0722 * pixel[0] + 0.7152 * pixel[1] + 0.2126 * pixel[2];
		}
	};

	cv::Mat textSubMat; //submatrix region of the original frame
	cv::Rect textBoxRect; //dimension rect of text box
	cv::Rect textRect; //adjusted dimension rect of text box to fit text height
	cv::Mat textMask; //mask of detected text in text box
	cv::Mat textMatLuminance; //Luminance of the text mat region
	std::string text; //text in the textbox
};

}
