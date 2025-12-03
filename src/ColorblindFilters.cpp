//Copyright (C) 2025 Electronic Arts, Inc.  All rights reserved.
#include "ColorblindFilters.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

namespace tik
{
	static cv::Mat crossProduct(const cv::Mat& a, const cv::Mat& b) {
		return (cv::Mat_<double>(3, 1) <<
			a.at<double>(1, 0) * b.at<double>(2, 0) - a.at<double>(2, 0) * b.at<double>(1, 0),
			a.at<double>(2, 0) * b.at<double>(0, 0) - a.at<double>(0, 0) * b.at<double>(2, 0),
			a.at<double>(0, 0) * b.at<double>(1, 0) - a.at<double>(1, 0) * b.at<double>(0, 0));
	}

	ColorblindFilters::ColorblindFilters(Configuration* config) {
		configuration = config;
		cv::Mat XYZJuddVosToLMS = configuration->getXYZJuddVosToLMSMatrix();
		LMSToLinearRGBMatrix = configuration->getLMSToLinearRGBMatrix();
		lmsNeutral = configuration->getLinearRGBToLMSMatrix() * cv::Mat::ones(3, 1, CV_64F);
		MELE = lmsNeutral.at<double>(1, 0) / lmsNeutral.at<double>(0, 0);

		// Tritanopia projection matrices
		n485 = crossProduct(lmsNeutral, XYZJuddVosToLMS * xyz485);
		n660 = crossProduct(lmsNeutral, XYZJuddVosToLMS * xyz660);
		projectionMatrix485 = (cv::Mat_<double>(3, 3) <<
			1.00000, 0.00000, 0.00000,
			0.00000, 1.00000, 0.00000,
			-(n485.at<double>(0, 0) / n485.at<double>(2, 0)), -(n485.at<double>(1, 0) / n485.at<double>(2, 0)), 0.00000);
		projectionMatrix660 = (cv::Mat_<double>(3, 3) <<
			1.00000, 0.00000, 0.00000,
			0.00000, 1.00000, 0.00000,
			-(n660.at<double>(0, 0) / n660.at<double>(2, 0)), -(n660.at<double>(1, 0) / n660.at<double>(2, 0)), 0.00000);
	}

	cv::Mat ColorblindFilters::sBGRToLinearRGB(const cv::Mat& sbgr)
	{
		cv::Mat linearRGB;
		cv::cvtColor(sbgr, linearRGB, cv::COLOR_BGR2RGB);
		cv::LUT(linearRGB, configuration->getSbgrValues(), linearRGB);
		return linearRGB;
	}

	cv::Mat ColorblindFilters::linearRGBToLMS(const cv::Mat& linearRGB) {
		cv::Mat lms;
		cv::transform(linearRGB, lms, configuration->getLinearRGBToLMSMatrix());
		return lms;
	}

	cv::Mat ColorblindFilters::LMSToLinearRGB(const cv::Mat& lmsImg) {
		cv::Mat linearRGB;
		cv::transform(lmsImg, linearRGB, LMSToLinearRGBMatrix);
		return linearRGB;
	}

	cv::Mat ColorblindFilters::linearRGBToSRGB(const cv::Mat& linearRGB)
	{
		cv::Mat sRGB;
		linearRGB.convertTo(sRGB, CV_32F);
		cv::cvtColor(sRGB, sRGB, cv::COLOR_RGB2BGR);
		std::vector<cv::Mat> channels;
		cv::split(sRGB, channels);

		// Find the most negative value per pixel across channels
		cv::Mat min_val;
		cv::min(channels[0], channels[1], min_val);
		cv::min(min_val, channels[2], min_val);
		cv::min(min_val, 0.0f, min_val);

		for (int c = 0; c < 3; ++c) {
			// desaturate to fit in gamut by adding white to all channels
			channels[c] = channels[c] - min_val;
			cv::min(channels[c], 1.0f, channels[c]);
			cv::max(channels[c], 0.0f, channels[c]);

			cv::Mat mask = channels[c] < 0.0031308f;
			cv::Mat higher, lower;
			cv::pow(channels[c], 1.0f / 2.4f, higher);
			higher = (1.055f * higher - 0.055f) * 255.0f;
			lower = channels[c] * 12.92f * 255.0f;
			channels[c] = higher;
			lower.copyTo(channels[c], mask);
		}

		cv::merge(channels, sRGB);
		return sRGB;
	}

	cv::Mat ColorblindFilters::protanopiaProjection(const cv::Mat& lmsImg)
	{
		cv::Mat protanImg;
		cv::transform(lmsImg, protanImg, configuration->getProtanProjectionMatrix());
		protanImg = LMSToLinearRGB(protanImg);
		protanImg = linearRGBToSRGB(protanImg);
		return protanImg;
	}

	cv::Mat ColorblindFilters::deuteranopiaProjection(const cv::Mat& lmsImg)
	{
		cv::Mat deutanImg;
		cv::transform(lmsImg, deutanImg, configuration->getDeutanProjectionMatrix());
		deutanImg = LMSToLinearRGB(deutanImg);
		deutanImg = linearRGBToSRGB(deutanImg);
		return deutanImg;
	}

	cv::Mat ColorblindFilters::tritanopiaProjection(const cv::Mat& lmsImg)
	{
		cv::Mat tritanImg;
		std::vector<cv::Mat> lmsChannels(3);
		cv::split(lmsImg, lmsChannels);
		cv::Mat& L = lmsChannels[0];
		cv::Mat& M = lmsChannels[1];

		// check which projection plane to use for each pixel
		cv::Mat ratio, mask;
		cv::divide(M, L, ratio);
		cv::compare(ratio, MELE, mask, cv::CMP_LT);

		cv::Mat proj485, proj660;
		cv::transform(lmsImg, proj485, projectionMatrix485);
		cv::transform(lmsImg, proj660, projectionMatrix660);

		cv::Mat S_485, S_660;
		cv::extractChannel(proj485, S_485, 2);
		cv::extractChannel(proj660, S_660, 2); 
		cv::Mat S = S_485;
		S_660.copyTo(S, mask);

		std::vector<cv::Mat> tritanChannels = { L, M, S };
		cv::merge(tritanChannels, tritanImg);
		cv::Mat linearRGB = LMSToLinearRGB(tritanImg);
		tritanImg = linearRGBToSRGB(linearRGB);

		return tritanImg;
	}

	std::vector<cv::Mat> ColorblindFilters::applyColorblindFilters(const cv::Mat& img) {
		cv::Mat linearRGB = sBGRToLinearRGB(img);
		cv::Mat lms = linearRGBToLMS(linearRGB);

		cv::Mat protanImg = protanopiaProjection(lms);
		protanImg.convertTo(protanImg, CV_8UC3);

		cv::Mat deutanImg = deuteranopiaProjection(lms);
		deutanImg.convertTo(deutanImg, CV_8UC3);

		cv::Mat tritanImg = tritanopiaProjection(lms);
		tritanImg.convertTo(tritanImg, CV_8UC3);

		cv::Mat grayImg;
		cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
		cv::cvtColor(grayImg, grayImg, cv::COLOR_GRAY2BGR);
		grayImg.convertTo(grayImg, CV_8UC3);

		return { protanImg, deutanImg, tritanImg, grayImg };
	}
}
