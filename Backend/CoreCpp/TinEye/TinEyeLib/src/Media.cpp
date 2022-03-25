#include "Media.h"
#include "Instrumentor.h"
#include "TinEye.h"
#include "Video.h"
#include "Image.h"

namespace tin {
	Media* Media::CreateMedia(fs::path path) {
		Media* media = nullptr;
		cv::Mat imgMat = cv::imread(path.string(), cv::IMREAD_COLOR);
		if (!imgMat.empty()) {
			media = new Image(path, imgMat);
		}
		else{
			cv::VideoCapture capture(path.string());
			if (capture.isOpened()) {
				media = new Video(path, capture);
			}
		}
		
		return media;
	}

	Media::~Media() {
		if (!imageMatrix.empty()) {
			imageMatrix.release();
		}
		if (!luminanceMap.empty()) {
			luminanceMap.release();
		}
	}

	cv::Mat Media::getImageMatrix()
	{
		return imageMatrix;
	}

	cv::Mat Media::getFrameLuminance() {
		//Make sure that image has been loaded and we haven't previously calculated the luminance already
		if (!imageMatrix.empty() && luminanceMap.empty()) {
			luminanceMap = TinEye::calculateLuminance(imageMatrix);
		}

		return luminanceMap;
	}

	void Media::highlightBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& matrix, int thickness)
	{
		PROFILE_FUNCTION();
		if (!matrix.empty()) {
			cv::line(matrix, cv::Point(x1, y1), cv::Point(x2, y1), color, thickness);
			cv::line(matrix, cv::Point(x2, y1), cv::Point(x2, y2), color, thickness);
			cv::line(matrix, cv::Point(x2, y2), cv::Point(x1, y2), color, thickness);
			cv::line(matrix, cv::Point(x1, y2), cv::Point(x1, y1), color, thickness);
		}
	}

	cv::Mat Media::calculateLuminanceHistogram(cv::Rect rect, cv::Rect ignoreRegion)
	{
		PROFILE_FUNCTION();
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

	cv::Mat Media::calculateLuminanceHistogram()
	{
		return calculateLuminanceHistogram();
	}

	void Media::flipLuminance(const int& x1, const int& y1, const int& x2, const int& y2)
	{
		PROFILE_FUNCTION();
		if (!luminanceMap.empty()) {
			cv::Mat subMatrix = luminanceMap(cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1));
			subMatrix = 1 - subMatrix;
		}
	}

	void Media::flipLuminance()
	{

		if (!luminanceMap.empty()) {
			flipLuminance(0, 0, luminanceMap.cols - 1, luminanceMap.rows - 1);
		}
	}


	void Media::displayLuminanceHistogram(cv::Mat histogram)
	{
		PROFILE_FUNCTION();
		cv::Mat histImage = generateLuminanceHistogramImage(histogram);
		cv::imshow("histogram", histImage);
		cv::waitKey();
	}

	void Media::saveLuminanceHistogram(cv::Mat histogram, std::string filepath)
	{
		cv::Mat histImage = generateLuminanceHistogramImage(histogram);
		cv::imwrite(filepath, histImage);
	}

	cv::Mat Media::generateLuminanceHistogramImage(cv::Mat histogram)
	{
		PROFILE_FUNCTION();
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

	void Media::saveHistogramCSV(cv::Mat histogram, std::string filename)
	{
		PROFILE_FUNCTION();
		std::ofstream filestream;
		filestream.open(filename.c_str());
		filestream << cv::format(histogram, cv::Formatter::FMT_CSV) << std::endl;
		filestream.close();
	}

	double Media::LuminanceMeanWithMask(const cv::Mat& mat, const cv::Mat& mask) {
		return cv::mean(mat, mask)[0];
	}

	void Media::saveOutputData(cv::Mat data, std::string name) {
		PROFILE_FUNCTION();
		fs::path outputPath = path.parent_path() / (path.filename().string() + "_output");

		if (!fs::is_directory(outputPath) || !fs::exists(outputPath)) {
			fs::create_directory(outputPath);
		}


		cv::imwrite(fs::path(outputPath / name).string(), data);
	}
}