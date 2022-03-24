#include "TextboxDetection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <boost/log/trivial.hpp>

#include "AppSettings.h"
#include "TextDetectionParams.h"

namespace tin {
	TextboxDetection::TextboxDetection(const TextDetectionParams* params)
	{
		east = new cv::dnn::TextDetectionModel_EAST("frozen_east_text_detection.pb");

		BOOST_LOG_TRIVIAL(trace) << "Confidence set to "
			<< params->getConfidenceThreshold() << std::endl;
		//Confidence on textbox threshold
		east->setConfidenceThreshold(params->getConfidenceThreshold());
		//Non Maximum supression
		east->setNMSThreshold(params->getNMSThreshold());

		east->setInputScale(params->getDetectionScale());

		//Default values from documentation are (123.68, 116.78, 103.94);
		auto mean = params->getDetectionMean();
		cv::Scalar detMean(mean[0], mean[1], mean[2]);
		east->setInputMean(detMean);

		east->setInputSwapRB(true);
	}

	TextboxDetection::~TextboxDetection() {
		if (east != nullptr) {
			delete east;
		}
	}

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

	std::vector<Textbox> TextboxDetection::detectBoxes(cv::Mat img, const AppSettings* appSettings, const TextDetectionParams* params)
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
		east->detect(resizedImg, detResults);

		BOOST_LOG_TRIVIAL(info) << "EAST found " << detResults.size() << " boxes\n";

		//Return smart pointer?

		//Transform points to original image size
		for (int i = 0; i < detResults.size(); i++) {
			for (int j = 0; j < detResults[i].size(); j++) {
				detResults[i][j].x /= widthRatio;
				detResults[i][j].y /= heightRatio;

				//Make sure points are within bounds
				detResults[i][j].x = std::min(std::max(0, detResults[i][j].x), img.cols);
				detResults[i][j].y = std::min(std::max(0, detResults[i][j].y), img.rows);
			}
		}

		std::vector<Textbox> boxes;
		for (std::vector<cv::Point > points : detResults) {
			if (HorizontalTiltAngle(points[1], points[2]) < params->getRotationThresholdRadians()) {
				boxes.emplace_back(points);
			}
			else {
				BOOST_LOG_TRIVIAL(trace) << "Ignoring tilted text in " << points[1] << std::endl;
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

	void TextboxDetection::mergeTextBoxes(std::vector<Textbox>& boxes, const TextDetectionParams* params) {

		std::pair<float, float> mergeThreshold = params->getMergeThreshold();

		for (auto boxIt = boxes.begin(); boxIt != boxes.end(); boxIt++) {
			for (auto targetIt = boxIt; targetIt != boxes.end(); ) {
				if (boxIt != targetIt) {
					auto overlap = Textbox::OverlapAxisPercentage(*boxIt, *targetIt);
					if (overlap.first >= mergeThreshold.first && overlap.second >= mergeThreshold.second) {
						BOOST_LOG_TRIVIAL(info) << boxIt->getRect() << "merges with " << targetIt->getRect() << std::endl;
						*boxIt = Textbox(boxIt->getRect() | targetIt->getRect());
						targetIt = boxes.erase(targetIt);
					}
					else {
						targetIt++;
					}
				}
				else {
					targetIt++;
				}
			}
		}

	}

	float TextboxDetection::HorizontalTiltAngle(const cv::Point& a, const cv::Point& b) {
		double hip = cv::norm(b - a);
		double h = std::abs(a.y - b.y);
		return asin(h / hip);
	}
}
