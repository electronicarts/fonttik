#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>
#include "Results.h"

namespace fs = std::filesystem;

namespace tin {
	class Media;
	class Frame {
		Media* parentMedia;
		int frameNumber;
		cv::Mat imageMatrix;
		cv::Mat luminanceMap;

		static cv::Mat generateLuminanceHistogramImage(cv::Mat histogram);

	public:
		Frame(Media* parent, int number,cv::Mat imageMat):
		parentMedia(parent), frameNumber(number),
		imageMatrix(imageMat),luminanceMap() {}
		virtual ~Frame();
		//Returns loaded image matrix
		cv::Mat getImageMatrix();

		//Returns loaded image's luminance map, if map hasn't been calculated calculates it as well
		cv::Mat getFrameLuminance();

		//Saves calculated luminance map to specified filepath
		void saveLuminanceMap(std::string filepath);

		//Returns average surrounding luminance of a given bounding box in luminance map
		uchar getAverageSurroundingLuminance(const int& x1, const int& y1, const int& x2, const int& y2, const int& marginX = 3, const int& marginY = 3);

		//Returns average surrounding luminance of a given bounding box in luminance map
		uchar getAverageSurroundingLuminance(cv::Rect region, const int& marginX = 3, const int& marginY = 3);

		//Highlights box in specified matrix
		static void highlightBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& matrix, int thickness = 1);

		//Puts output value from a resultbox next to itself in image
		static void putResultBoxValues(cv::Mat& matrix, ResultBox& box, int precision);

		//Calculates the luminance histogram of a region
		cv::Mat calculateLuminanceHistogram(cv::Rect, cv::Rect ignoreRegion = cv::Rect(0, 0, 0, 0));

		//Calculates the luminance histogram of an image
		cv::Mat calculateLuminanceHistogram();

		static void displayLuminanceHistogram(cv::Mat histogram);

		static void saveLuminanceHistogram(cv::Mat histogram, std::string filepath);

		//Saves specified luminance histogram to a csv file
		static void saveHistogramCSV(cv::Mat histogram, std::string filename);

		//Saves the data in the image sub folder
		void saveOutputData(cv::Mat data, fs::path path);

		/*
		Operators and Calculations
		*/
		//Flips the luminance of a given region
		void flipLuminance(const int& x1, const int& y1, const int& x2, const int& y2);

		//Flips the luminance of the whole image
		void flipLuminance();

		static double LuminanceMeanWithMask(const cv::Mat& mat, const cv::Mat& mask);

		//Returns current frame number identifier, by default is 0
		virtual size_t getFrameNumber() { return frameNumber; }

		//Path of the original image or the video its coming from
		fs::path getPath();
	};
}