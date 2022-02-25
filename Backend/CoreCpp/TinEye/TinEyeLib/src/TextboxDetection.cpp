#include "TextboxDetection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <boost/log/trivial.hpp>


void TextboxDetection::fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result)
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

std::vector<Textbox> TextboxDetection::detectBoxes(cv::Mat img, bool saveBoxesImage)
{
	//Calculate needed conversion for new width and height to be multiples of 32
	////This needs to be multiple of 32
	int inpWidth = 32 * (img.cols / 32 + ((img.cols % 32 != 0) ? 1 : 0));
	int inpHeight = 32 * (img.rows / 32 + ((img.rows % 32 != 0) ? 1 : 0));
	float widthRatio = float(inpWidth) / img.cols;
	float heightRatio = float(inpHeight) / img.rows;

	cv::dnn::TextDetectionModel_EAST east("frozen_east_text_detection.pb");
	east.setConfidenceThreshold(0.5);
	east.setNMSThreshold(0.4);

	// Parameters for Detection
	double detScale = 1.0;
	cv::Size detInputSize = cv::Size(inpWidth, inpHeight);
	cv::Scalar detMean = cv::Scalar(123.68, 116.78, 103.94);
	bool swapRB = true;
	east.setInputParams(detScale, detInputSize, detMean, swapRB);

	//Image reading
	cv::Mat resizedImg;
	cv::resize(img, resizedImg, detInputSize);

	std::vector< std::vector<cv::Point> > detResults;
	east.detect(resizedImg, detResults);

	BOOST_LOG_TRIVIAL(info) << "EAST found " << detResults.size() << "boxes\n";

	//Return smart pointer?

	//Transform points to original image size
	for (int i = 0; i < detResults.size(); i++) {
		for (int j = 0; j < detResults[i].size(); j++) {
			detResults[i][j].x /= widthRatio;
			detResults[i][j].y /= heightRatio;
		}
	}

	std::vector<Textbox> boxes;
	for (std::vector<cv::Point > points : detResults) {
		boxes.emplace_back(points);
	}

	//Points are
	/*
	[1]---------[2]
	|            |
	|            |
	[0]---------[3]
	*/

	if (saveBoxesImage) {
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
		cv::imwrite("resources/textbox_results.png", recInput);
	}

	return boxes;
}
