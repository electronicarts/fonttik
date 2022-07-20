//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "TextboxDetectionEAST.h"
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "Log.h"
#include "AppSettings.h"
#include "TextDetectionParams.h"
#include "Instrumentor.h"

namespace tin {
	void TextboxDetectionEAST::init(const TextDetectionParams* params, const AppSettings* appSettingsCfg)
	{
		detectionParams = params;
		appSettings = appSettingsCfg;

		const EASTDetectionParams* eastParams = params->getEASTParams();

		east = new cv::dnn::TextDetectionModel_EAST(eastParams->getDetectionModel());

		LOG_CORE_TRACE("Confidence set to {0}", detectionParams->getConfidenceThreshold());
		//Confidence on textbox threshold
		east->setConfidenceThreshold(detectionParams->getConfidenceThreshold());
		//Non Maximum supression
		east->setNMSThreshold(eastParams->getNMSThreshold());

		east->setInputScale(eastParams->getDetectionScale());

		//Default values from documentation are (123.68, 116.78, 103.94);
		auto mean = eastParams->getDetectionMean();
		cv::Scalar detMean(mean[0], mean[1], mean[2]);
		east->setInputMean(detMean);

		east->setInputSwapRB(true);
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

	std::vector<Textbox> TextboxDetectionEAST::detectBoxes(const cv::Mat& img)
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
			PROFILE_SCOPE("EAST DNN");
			east->detect(resizedImg, detResults);
		}

		LOG_CORE_INFO("EAST found {0} boxes", detResults.size());

		//Transform points to original image size
		{
			PROFILE_SCOPE("Point scaling");
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

		std::vector<Textbox> boxes;
		for (std::vector<cv::Point > points : detResults) {
			if (HorizontalTiltAngle(points[1], points[2]) < detectionParams->getRotationThresholdRadians()) {
				boxes.emplace_back(points);
			}
			else {
				//LOG_CORE_TRACE("Ignoring tilted text in {0}", points[1]);
			}
		}

		//Points are
		/*
		[1]---------[2]
		|            |
		|            |
		[0]---------[3]
		*/

#ifdef _DEBUG

		if (appSettings->saveRawTexboxOutline()) {
			// Text Recognition
			cv::Mat recInput = img.clone();
			if (detResults.size() > 0) {
				std::vector< std::vector<cv::Point> > contours;
				for (uint i = 0; i < detResults.size(); i++)
				{
					const auto& quadrangle = detResults[i];
					CV_CheckEQ(quadrangle.size(), (size_t)4, "");

					contours.emplace_back(quadrangle);

					std::vector<cv::Point2f> quadrangle_2f;
					for (int j = 0; j < 4; j++) {
						quadrangle_2f.emplace_back(quadrangle[j]);
					}

					cv::Mat cropped;
					fourPointsTransform(recInput, &quadrangle_2f[0], cropped);
				}
				polylines(recInput, contours, true, cv::Scalar(0, 255, 0), 2);
			}
			cv::imwrite("resources/raw_EAST_textboxes.png", recInput);
		}

#endif // _DEBUG

		return boxes;
	}
}
