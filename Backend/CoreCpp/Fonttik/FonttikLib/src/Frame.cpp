//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "Media.h"
#include "Fonttik.h"
#include "Video.h"
#include "Image.h"
#include "Media.h"
#include <fstream>

namespace tik {
	Frame::~Frame() {
		if (!imageMatrix.empty()) {
			imageMatrix.release();
		}
		if (!luminanceMap.empty()) {
			luminanceMap.release();
		}
	}

	cv::Mat Frame::getImageMatrix()
	{
		return imageMatrix;
	}

	cv::Mat Frame::getFrameLuminance() {
		//Make sure that image has been loaded and we haven't previously calculated the luminance already
		if (!imageMatrix.empty() && luminanceMap.empty()) {
			luminanceMap = Fonttik::calculateLuminance(imageMatrix);
		}

		return luminanceMap;
	}

	void Frame::highlightBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& matrix, int thickness)
	{
		if (!matrix.empty()) {
			cv::line(matrix, cv::Point(x1, y1), cv::Point(x2, y1), color, thickness);
			cv::line(matrix, cv::Point(x2, y1), cv::Point(x2, y2), color, thickness);
			cv::line(matrix, cv::Point(x2, y2), cv::Point(x1, y2), color, thickness);
			cv::line(matrix, cv::Point(x1, y2), cv::Point(x1, y1), color, thickness);
		}
	}

	void Frame::putResultBoxValues(cv::Mat& matrix, ResultBox& box, int precision) {
		std::stringstream stream;
		stream << std::fixed << std::setprecision(precision) << box.value;
		std::string text = stream.str();

		//Calculate text size
		int baseline;
		cv::Size textSize = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 1, 3, &baseline);

		//Check to see if text fits to the right of the textbox, if not, put it to the left
		cv::Point originPoint = (box.x + box.width + textSize.width < matrix.cols) ?
			cv::Point(box.x + box.width, box.y + box.height) : cv::Point(box.x - textSize.width, box.y + box.height);

		//Put the text into the image
		cv::putText(matrix, text, originPoint, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(), 3);
		cv::putText(matrix, text, originPoint, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 1);
	}

	cv::Mat Frame::calculateLuminanceHistogram(cv::Rect rect, cv::Rect ignoreRegion)
	{
		if (luminanceMap.empty()) {
			getFrameLuminance();
		}

		cv::Mat region = luminanceMap(rect);
		cv::Mat mask = cv::Mat::ones(region.rows, region.cols, CV_8UC1) * 255;
		cv::rectangle(mask, ignoreRegion, cv::Scalar(0, 0, 0, 0), cv::FILLED);
		cv::Mat histogram;

		int histSize = 256;
		float range[] = { 0,256 };
		const float* histRange[] = { range };

		cv::calcHist(&region, 1, 0, mask, histogram, 1, &histSize, histRange, true, false);

		return histogram;
	}

	cv::Mat Frame::calculateLuminanceHistogram()
	{
		return calculateLuminanceHistogram();
	}

	void Frame::flipLuminance(const int& x1, const int& y1, const int& x2, const int& y2)
	{
		if (!luminanceMap.empty()) {
			cv::Mat subMatrix = luminanceMap(cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1));
			subMatrix = 1 - subMatrix;
		}
	}

	void Frame::flipLuminance()
	{

		if (!luminanceMap.empty()) {
			flipLuminance(0, 0, luminanceMap.cols - 1, luminanceMap.rows - 1);
		}
	}


	void Frame::displayLuminanceHistogram(cv::Mat histogram)
	{
		cv::Mat histImage = generateLuminanceHistogramImage(histogram);
		cv::imshow("histogram", histImage);
		cv::waitKey();
	}

	void Frame::saveLuminanceHistogram(cv::Mat histogram, std::string filepath)
	{
		cv::Mat histImage = generateLuminanceHistogramImage(histogram);
		cv::imwrite(filepath, histImage);
	}

	cv::Mat Frame::generateLuminanceHistogramImage(cv::Mat histogram)
	{
		//Display the histogram
		int hist_w = 512, hist_h = 400;
		int bin_w = cvRound((double)hist_w / 256);
		cv::Mat histImage(hist_h, hist_w, CV_8UC1, cv::Scalar(0, 0, 0));

		cv::normalize(histogram, histogram, 0, histImage.rows, cv::NORM_MINMAX);

		for (int i = 1; i < 256; i++) {
			line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(histogram.at<float>(i - 1))), cv::Point(bin_w * (i), hist_h - cvRound(histogram.at<float>(i))), cv::Scalar(255), 1, 8, 0);
		}


		return histImage;
	}

	void Frame::saveHistogramCSV(cv::Mat histogram, std::string filename)
	{
		std::ofstream filestream;
		filestream.open(filename.c_str());
		filestream << cv::format(histogram, cv::Formatter::FMT_CSV) << std::endl;
		filestream.close();
	}

	double Frame::LuminanceMeanWithMask(const cv::Mat& mat, const cv::Mat& mask) {
		return cv::mean(mat, mask)[0];
	}

	void Frame::saveOutputData(cv::Mat data, fs::path path) {
		cv::imwrite(path.string(), data);
	}

	fs::path Frame::getPath() {
		return parentMedia->getPath(); 
	}

	Media* Frame::getMedia() {
		return parentMedia;
	}
}