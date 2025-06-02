//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "TextboxDetectionEAST.h"
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "fonttik/Log.h"
#include "fonttik/ConfigurationParams.hpp"

#include <random>


namespace tik {
	void TextboxDetectionEAST::init(const std::vector<double>& sRGB_LUT)
	{
		//Store sRGB Look up table
		this->sRGB_LUT = sRGB_LUT;

		east = new cv::dnn::TextDetectionModel_EAST(detectionParams->eastParams.detectionModel);

		LOG_CORE_TRACE("Confidence set to {0}", detectionParams->confidenceThreshold);
		//Confidence on textbox threshold
		east->setConfidenceThreshold(detectionParams->confidenceThreshold);
		//Non Maximum supression
		east->setNMSThreshold(detectionParams->eastParams.nonMaxSuprresionThreshold);

		east->setInputScale(detectionParams->eastParams.detectionScale);

		//Default values from documentation are (123.68, 116.78, 103.94);
		auto mean = detectionParams->eastParams.detectionMean;
		cv::Scalar detMean(mean[0], mean[1], mean[2]);
		east->setInputMean(detMean);

		east->setInputSwapRB(true);

		east->setPreferableBackend((cv::dnn::Backend)detectionParams->preferredBackend);
		east->setPreferableTarget((cv::dnn::Target)detectionParams->preferredTarget);
	}

	TextboxDetectionEAST::~TextboxDetectionEAST() {
		if (east != nullptr) {
			delete east;
		}
	}

	void TextboxDetectionEAST::fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result)
	{
		const cv::Size outputSize = cv::Size(100, 32);

		cv::Point2f targetVertices[4] = {
			cv::Point(0, outputSize.height - 1),
			cv::Point(0, 0), cv::Point(outputSize.width - 1, 0),
			cv::Point(outputSize.width - 1, outputSize.height - 1)
		};
		cv::Mat rotationMatrix = getPerspectiveTransform(vertices, targetVertices);

		warpPerspective(frame, result, rotationMatrix, outputSize);
	}

	std::vector<TextBox> TextboxDetectionEAST::detectBoxes(const cv::Mat& img)
	{
		//Calculate needed conversion for new width and height to be multiples of 32
		////This needs to be multiple of 32
		int inpWidth = 32 * (img.cols / 32 + ((img.cols % 32 != 0) ? 1 : 0));
		int inpHeight = 32 * (img.rows / 32 + ((img.rows % 32 != 0) ? 1 : 0));
		float widthRatio = float(inpWidth) / img.cols;
		float heightRatio = float(inpHeight) / img.rows;

		cv::Size detInputSize = cv::Size(inpWidth, inpHeight);

		east->setInputSize(detInputSize);

		//Image reading
		cv::Mat resizedImg;
		cv::resize(img, resizedImg, detInputSize);

		std::vector< std::vector<cv::Point> > detResults;
		{
			east->detect(resizedImg, detResults);
		}

		LOG_CORE_TRACE("DB_EAST found {0} boxes", detResults.size());

		//Transform points to original image size
		{
			for (int i = 0; i < detResults.size(); i++) {
				for (int j = 0; j < detResults[i].size(); j++) {
					detResults[i][j].x /= widthRatio;
					detResults[i][j].y /= heightRatio;

					//Make sure points are within bounds
					detResults[i][j].x = std::min(std::max(0, detResults[i][j].x), img.cols);
					detResults[i][j].y = std::min(std::max(0, detResults[i][j].y), img.rows);
				}
			}
		}

		std::vector<TextBox> boxes;

		for (std::vector<cv::Point > points : detResults) 
		{
			if (HorizontalTiltAngle(points[1], points[2]) < detectionParams->rotationThresholdRadians) 
			{
				boxes.emplace_back(TextBox{ points, img });
			}
			else 
			{
				LOG_CORE_TRACE("Ignoring tilted text in {0}", points[1]);
			}
		}

		return boxes;
	}

	LinesAndWords TextboxDetectionEAST::detectLinesAndWords(const cv::Mat& img)
	{
		auto boxes = detectBoxes(img);

		//merge lines
		const int MAX_Y_DIFF = 5;
		const int MAX_X_DIFF = 50;

		std::vector<TextBox> mergedLines;
		if (boxes.empty()) return { mergedLines, boxes };

		// Sort boxes by the top y-coordinate
		auto sortedBoxes = boxes;
		for (auto& textBox : sortedBoxes)
		{
			textBox.calculateTextBoxLuminance(sRGB_LUT);
		}
		for (auto& textBox : sortedBoxes)
		{
			textBox.calculateTextMask();
			auto boxRect = textBox.getTextBoxRect();
			auto textRect = textBox.getTextRect();

			int x = std::max(boxRect.x + textRect.x , 0);
			int y = std::max(boxRect.y + textRect.y , 0);
			int w = std::min(textRect.width  , (img.cols - x));
			int h = std::min(textRect.height , (img.rows - y));

			auto tb = cv::Rect{x , y, w, h };
			textBox = { tb,img };
		}

		// Sort vertically
		std::sort(sortedBoxes.begin(), sortedBoxes.end(), [](const TextBox& a, const TextBox& b) {
			return a.getTextBoxRect().y < b.getTextBoxRect().y; 
		});
		// Sort horizontally after vertically
		std::sort(sortedBoxes.begin(), sortedBoxes.end(), [&](const TextBox& a, const TextBox& b) {
			if (std::abs(a.getTextBoxRect().y - b.getTextBoxRect().y) < MAX_Y_DIFF) {
				return a.getTextBoxRect().x < b.getTextBoxRect().x;
			}
			return a.getTextBoxRect().y < b.getTextBoxRect().y;
		});

		TextBox currentLine = sortedBoxes[0];
		int count = 0;

		for (size_t i = 1; i < sortedBoxes.size(); ++i) {
			const auto& box = sortedBoxes[i];

			// Calculate the average y-value for alignment
			int heightThreshold = (currentLine.getTextBoxRect().height + box.getTextBoxRect().height) / 2;
			int avgYCurrent = currentLine.getTextBoxRect().y + currentLine.getTextBoxRect().height / 2;
			int avgYBox = box.getTextBoxRect().y + box.getTextBoxRect().height / 2;

			auto currentX = currentLine.getTextBoxRect().x + currentLine.getTextBoxRect().width;
			auto mergeX = (std::abs(box.getTextBoxRect().x- currentX) <=  + MAX_X_DIFF);
			// Check if the current box is aligned with the current line
			if (false || std::abs(avgYBox - avgYCurrent) < MAX_Y_DIFF  &&  mergeX)
			{
				// Merge text
				// Expand bounding rectangle
				currentLine = { currentLine.getTextBoxRect() | box.getTextBoxRect(), img };
				count++;
			}
			else {
				// Save the current line and start a new one
				//std::cout << "The following text: "<< box.getTextBoxRect() << (std::abs(avgYBox - avgYCurrent) < MAX_Y_DIFF ?" failed on X":"failed on Y" )<< std::endl;
				mergedLines.push_back(currentLine);
				currentLine = box;
			}
		}
		
		return { mergedLines, boxes };
	}
}
