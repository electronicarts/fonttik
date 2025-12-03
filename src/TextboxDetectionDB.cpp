// Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.
#include "TextboxDetectionDB.h"
#include "fonttik/ConfigurationParams.hpp"
#include "fonttik/Log.h"
#include <random>


namespace tik {
	void TextboxDetectionDB::init(const std::vector<double>& sRGB_LUT)
	{
		//Store sRGB Look up table
		this->sRGB_LUT = sRGB_LUT;

		//Models can be found in https://github.com/opencv/opencv/blob/master/doc/tutorials/dnn/dnn_text_spotting/dnn_text_spotting.markdown
		db = new cv::dnn::TextDetectionModel_DB(detectionParams->dbParams.detectionModel);

		// Post-processing parameters
		db->setBinaryThreshold(detectionParams->dbParams.binThresh)
			.setPolygonThreshold(detectionParams->dbParams.polyThresh)
			.setMaxCandidates(detectionParams->dbParams.maxCandidates)
			.setUnclipRatio(detectionParams->dbParams.unclipRatio)
			;

		// Normalization parameters
		//Not the same as in EASt
		double scale = detectionParams->dbParams.scale;
		//Default values from documentation are (123.68, 116.78, 103.94);
		auto mean = detectionParams->dbParams.mean;
		cv::Scalar detMean(mean[0], mean[1], mean[2]);

		// The input shape
		auto size = detectionParams->dbParams.inputSize;
		cv::Size inputSize = cv::Size(size[0], size[1]);

		db->setInputParams(scale, inputSize, detMean);

		db->setPreferableBackend((cv::dnn::Backend)detectionParams->preferredBackend);
		db->setPreferableTarget((cv::dnn::Target)detectionParams->preferredTarget);
	}

	TextboxDetectionDB::~TextboxDetectionDB() {
		if (db != nullptr) {
			delete db;
		}
		db = nullptr;
	} 

	std::vector<TextBox> TextboxDetectionDB::detectBoxes(const cv::Mat& img) {

		std::vector< std::vector<cv::Point> > detResults;
		{
			db->detect(img, detResults);
		}

		for (int i = 0; i < detResults.size(); i++) 
		{
			for (int j = 0; j < detResults[i].size(); j++) 
			{

				//Make sure points are within bounds
				detResults[i][j].x = std::min(std::max(0, detResults[i][j].x), img.cols);
				detResults[i][j].y = std::min(std::max(0, detResults[i][j].y), img.rows);
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

		//Points are
		/*
		[1]---------[2]
		|            |
		|            |
		[0]---------[3]
		*/

		return boxes;
	}

	LinesAndWords TextboxDetectionDB::detectLinesAndWords(const cv::Mat& img)
	{
		auto boxes = detectBoxes(img);

		//merge lines
		const int MAX_Y_DIFF = 10;
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

			auto tb = cv::Rect{ x , y, w, h };

			textBox = {tb, img };
		}

		// boxRect.x + textRect.x - 1, boxRect.y + textRect.y - 1, textRect.width + 2, textRect.height + 2
		std::sort(sortedBoxes.begin(), sortedBoxes.end(), [](const TextBox& a, const TextBox& b) {
			return a.getTextBoxRect().y < b.getTextBoxRect().y;
		});
		std::sort(sortedBoxes.begin(), sortedBoxes.end(), [&](const TextBox& a, const TextBox& b) {
			if (std::abs(a.getTextBoxRect().y - b.getTextBoxRect().y) < MAX_Y_DIFF) {
				return a.getTextBoxRect().x < b.getTextBoxRect().x;
			}
			return a.getTextBoxRect().y < b.getTextBoxRect().y;
		});

		TextBox currentLine = sortedBoxes[0];

		for (size_t i = 1; i < sortedBoxes.size(); ++i) {
			const auto& box = sortedBoxes[i];

			// Calculate the average y-value for alignment
			int heightThreshold = (currentLine.getTextBoxRect().height + box.getTextBoxRect().height) / 2;
			int avgYCurrent = currentLine.getTextBoxRect().y + currentLine.getTextBoxRect().height / 2;
			int avgYBox = box.getTextBoxRect().y + box.getTextBoxRect().height / 2;

			// Check if the current box is aligned with the current line
			if (std::abs(avgYBox - avgYCurrent) < heightThreshold &&
				(box.getTextBoxRect().x <= currentLine.getTextBoxRect().x + currentLine.getTextBoxRect().width + MAX_X_DIFF)) {
				// Merge text
				// Expand bounding rectangle
				currentLine = { currentLine.getTextBoxRect() | box.getTextBoxRect(), img };
			}
			else {
				// Save the current line and start a new one
				//std::cout << "The following text: "<< box.getTextBoxRect() << (std::abs(avgYBox - avgYCurrent) < MAX_Y_DIFF ?" failed on X":"failed on Y" )<< std::endl;
				mergedLines.push_back(currentLine);
				currentLine = box;
			}
		}

		// Add the last line
		mergedLines.push_back(currentLine);

		return { mergedLines, boxes };
	}


}