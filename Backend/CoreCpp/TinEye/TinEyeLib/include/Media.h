//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>
#include "Results.h"

namespace fs = std::filesystem;


namespace tin {
	class Frame;
	class Media {
	protected:
		cv::Mat imageMatrix; //Image matrix for the current frame.
		fs::path path; //Location of the original file, used for creating a sub-directory with the results

		Results results;


		static cv::Mat generateLuminanceHistogramImage(cv::Mat histogram);
		
		Media(fs::path filePath) :path(filePath) {}
	public:
		virtual ~Media();
		//Factory Method that creates a video or an image depending on the file
		//returns nullptr in case of invalid file
		static Media* CreateMedia(fs::path path);

		/// <summary>
		/// gets a copy of the current frame, USER IS RESPONSIBLE FOR DELETION
		/// </summary>
		/// <returns></returns>
		virtual Frame* getFrame() = 0;
		//If loaded file is a video grabs the next frame and returns true, if no frame available or file is an image returns false
		virtual bool nextFrame() = 0;

		//Generates outlines for the image's results and saves them
		virtual void saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, const std::vector<cv::Scalar>& colors, bool saveNumbers = false) = 0;

		//Path of the original image or the video its coming from
		fs::path getPath() { return path; };

		//Saves the data in the image sub folder
		void saveOutputData(cv::Mat data, fs::path path);

		//Returns output path and if it doesn't exist creates it
		fs::path getOutputPath();

		//Returns a pointer to the image's results struct for editing or reviewing
		Results* getResultsPointer() { return &results; }

		/*
		Operators and Calculations
		*/
		//Returns current frame number identifier, by default is 0
		virtual int getFrameCount() { return 0; }
	};
}