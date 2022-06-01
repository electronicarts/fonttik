#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>
#include "Results.h"

namespace fs = std::filesystem;


namespace tin {
	class Media {
	protected:
		cv::Mat imageMatrix;
		cv::Mat luminanceMap;
		fs::path path;

		Results results;


		static cv::Mat generateLuminanceHistogramImage(cv::Mat histogram);
		
		Media(fs::path filePath) :path(filePath) {}
	public:
		virtual ~Media();
		//Factory Method that creates a video or an image depending on the file
		//returns nullptr in case of invalid file
		static Media* CreateMedia(fs::path path);

		//If loaded file is a video grabs the next frame and returns true, if no frame available or file is an image returns false
		virtual bool nextFrame() = 0;

		//Generates outlines for the image's results and saves them
		virtual void saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, bool saveNumbers = false) = 0;

		//Path of the original image or the video its coming from
		fs::path getPath() { return path; };

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

		//Returns output path and if it doesn't exist creates it
		fs::path getOutputPath();

		//Returns a pointer to the image's results struct for editing or reviewing
		Results* getResultsPointer() { return &results; }

		/*
		Operators and Calculations
		*/
		//Flips the luminance of a given region
		void flipLuminance(const int& x1, const int& y1, const int& x2, const int& y2);

		//Flips the luminance of the whole image
		void flipLuminance();

		static double LuminanceMeanWithMask(const cv::Mat& mat, const cv::Mat& mask);

		//Returns current frame number identifier, by default is 0
		virtual int getFrameCount() { return 0; }
	};
}