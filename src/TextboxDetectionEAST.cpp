//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

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
		const int inpWidth = 32 * (img.cols / 32 + ((img.cols % 32 != 0) ? 1 : 0));
		const int inpHeight = 32 * (img.rows / 32 + ((img.rows % 32 != 0) ? 1 : 0));
		const float widthRatio = float(inpWidth) / img.cols;
		const float heightRatio = float(inpHeight) / img.rows;

		const cv::Size detInputSize = cv::Size(inpWidth, inpHeight);

		east->setInputSize(detInputSize);
		east->setConfidenceThreshold(detectionParams->confidenceThreshold);

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

		// Remove big boxes as they will be detected separately
		int minHeight = 40;
		if (img.rows == 1080) {
			minHeight = 60;
		}
		else if (img.rows >= 2160) {
			minHeight = 120;
		}

		detResults.erase(std::remove_if(detResults.begin(), detResults.end(),
			[&minHeight](const std::vector<cv::Point>& box) {
				return cv::boundingRect(box).height > minHeight;
			}), detResults.end());		

		// Big text detection
		cv::Size bigTextInputSize = cv::Size(736, 384);
		cv::resize(img, resizedImg, bigTextInputSize);
		east->setInputSize(bigTextInputSize);
		east->setConfidenceThreshold(0.75);

		std::vector< std::vector<cv::Point> > bigTextResults;
		{
			east->detect(resizedImg, bigTextResults);
		}

		LOG_CORE_TRACE("DB_EAST found {0} big boxes", bigTextResults.size());

		const float bigTextWidthRatio = float(bigTextInputSize.width) / img.cols;
		const float bigTextHeightRatio = float(bigTextInputSize.height) / img.rows;
		//Transform points to original image size
		{
			for (int i = 0; i < bigTextResults.size(); i++) {
				for (int j = 0; j < bigTextResults[i].size(); j++) {
					bigTextResults[i][j].x /= bigTextWidthRatio;
					bigTextResults[i][j].y /= bigTextHeightRatio;

					//Make sure points are within bounds
					bigTextResults[i][j].x = std::min(std::max(0, bigTextResults[i][j].x), img.cols);
					bigTextResults[i][j].y = std::min(std::max(0, bigTextResults[i][j].y), img.rows);
				}
			}
		}

		// Merge results
		for (const auto& bigText : bigTextResults)
		{
			// Ignore big text boxes that are too small
			if (cv::boundingRect(bigText).height < minHeight) {
				continue;
			}
			bool found = false;
			for (auto& detResult : detResults)
			{
				if (detResult.size() == 4 && cv::boundingRect(detResult).contains(cv::Point(bigText[0].x, bigText[0].y)))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				detResults.push_back(bigText);
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
		double MAX_Y_DIFF = 10.0;
		if (img.rows == 720) {
			MAX_Y_DIFF = 5;
		}
		if (img.rows == 1080) {
			MAX_Y_DIFF = 10;
		}
		if (img.rows >= 2160) {
			MAX_Y_DIFF = 20;
		}
		const int TEXT_BOX_BUFFER = 4;
		const int MAX_X_DIFF = 50;

		std::vector<TextBox> mergedLines;
		if (boxes.empty()) return { mergedLines, boxes };

		// Sort boxes by the top y-coordinate
		auto& sortedBoxes = boxes;
		for (auto& textBox : sortedBoxes)
		{
			textBox.calculateTextBoxLuminance(sRGB_LUT);
		}
		for (auto& textBox : sortedBoxes)
		{
			textBox.calculateTextMask();
			auto boxRect = textBox.getTextBoxRect();
			auto textRect = textBox.getTextRect();

			int x = std::max(boxRect.x + textRect.x, 0);
			int y = std::max(boxRect.y + textRect.y, 0);
			int w = std::min(boxRect.width, (img.cols - x));
			int h = std::min(boxRect.height, (img.rows - y));

			auto tb = cv::Rect{x , y, w, h };
			textBox = { tb,img };
		}

		// Sort vertically
		std::sort(sortedBoxes.begin(), sortedBoxes.end(), [](const TextBox& a, const TextBox& b) {
			return a.getTextBoxRect().y < b.getTextBoxRect().y; 
		});

		// Group into lines by similar y values
		std::vector<std::vector<TextBox>> lines;
		for (const auto& box : sortedBoxes) {
			bool added = false;
			for (auto& line : lines) {
				// Compare to last box in the line
				if (std::abs(box.getTextBoxRect().y - line.back().getTextBoxRect().y) < MAX_Y_DIFF) {
					line.push_back(box);
					added = true;
					break;
				}
			}
			if (!added) {
				lines.push_back({ box });
			}
		}

		// Sort each line by x
		for (auto& line : lines) {
			std::sort(line.begin(), line.end(), [](const TextBox& a, const TextBox& b) {
				return a.getTextBoxRect().x < b.getTextBoxRect().x;
			});
		}

		// Flatten back to a single vector
		std::vector<TextBox> finalSorted;
		for (const auto& line : lines) {
			finalSorted.insert(finalSorted.end(), line.begin(), line.end());
		}
		sortedBoxes = std::move(finalSorted);

		TextBox currentLine = sortedBoxes[0];
		int count = 0;

		for (size_t i = 1; i < sortedBoxes.size(); ++i) {
			const auto& box = sortedBoxes[i];

			auto currentRect = currentLine.getTextBoxRect();
			auto boxRect = box.getTextBoxRect();

			// Y alignment
			int middleYCurrent = currentRect.y + currentRect.height/2;
			int middleYBox = boxRect.y + boxRect.height/2;
			auto mergeY = (middleYBox > currentRect.y) && (middleYBox < (currentRect.y + currentRect.height));

			// X alignment
			int right = boxRect.x - (currentRect.x + currentRect.width);
			int left = currentRect.x - (boxRect.x + boxRect.width);
			auto mergeX = (right >= 0 && right <= MAX_X_DIFF) || (left >= 0 && left <= MAX_X_DIFF) || (right < 0 && left < 0);

			// Similar height
			auto mergeHeight = (std::abs(currentRect.height - boxRect.height) <= currentRect.height * 0.5) || (boxRect.height <= currentRect.height);

			// Check if the current box is aligned with the current line
			if (mergeY && mergeX && mergeHeight)
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
			// If we are at the last box, we need to add the current line
			if (i == sortedBoxes.size() - 1) {
				mergedLines.push_back(currentLine);
			}
		}

		return { mergedLines, boxes };
	}
}
