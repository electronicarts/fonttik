//Copyright (C) 2025 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include <opencv2/core.hpp>
#include "fonttik/Configuration.hpp"

namespace tik
{
    class ColorblindFilters {
    public:
        ColorblindFilters(Configuration* config);

        virtual ~ColorblindFilters() {}

        std::vector<cv::Mat> applyColorblindFilters(const cv::Mat& frame);
        
    private:
        Configuration* configuration;

        // XYZ Judd-Vos coordinates for 485nm and 660 nm wavelengths
		// Values taken from DaltonLens which they took from http://www.cvrl.org/
        const cv::Mat xyz485 = (cv::Mat_<double>(3, 1) << 0.05699, 0.16987, 0.5864);
        const cv::Mat xyz660 = (cv::Mat_<double>(3, 1) << 0.16161, 0.061, 0.00001);

        cv::Mat lmsNeutral, n485, n660;
		cv::Mat LMSToLinearRGBMatrix;
        double MELE; // Threshold to choose which tritanopia plane to use: M/L values for neutral point E
		cv::Mat projectionMatrix485, projectionMatrix660;

        cv::Mat sBGRToLinearRGB(const cv::Mat& sbgr);
        cv::Mat linearRGBToLMS(const cv::Mat& linearRGB);
        cv::Mat LMSToLinearRGB(const cv::Mat& lmsImg);
        cv::Mat linearRGBToSRGB(const cv::Mat& linearRGB);
        cv::Mat protanopiaProjection(const cv::Mat& lmsImg);
        cv::Mat deuteranopiaProjection(const cv::Mat& lmsImg);
        cv::Mat tritanopiaProjection(const cv::Mat& lmsImg);
    };
}
