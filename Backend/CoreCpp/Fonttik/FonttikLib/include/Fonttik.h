//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <filesystem>
#include "Media.h"
#include "Frame.h"
#include "Textbox.h"
#include <gtest/gtest.h>

namespace fs = std::filesystem;

namespace tik {
	
class Configuration;
class ITextboxDetection;
class ITextboxRecognition;
class IChecker;
class Frame;

class Fonttik {

public:
	Fonttik();
	Fonttik(const Fonttik& other);
	//Overloaded constructor that automatically calls init with provided configuration
	Fonttik(Configuration* configuration) { init(configuration); }
	~Fonttik();
	void init(Configuration* configuration);

	std::vector<Textbox> getTextBoxes(Frame& image);
	void mergeTextBoxes(std::vector<Textbox>& textBoxes);

	void applyFocusMask(Frame& frame);

	FrameResults fontSizeCheck(Frame& frame, std::vector<Textbox>& boxes);
	FrameResults textContrastCheck(Frame& frame, std::vector<Textbox>& boxes);
	Results* processMedia(Media& media);
	std::pair<FrameResults,FrameResults> processFrame(Frame* frame);
	Configuration* getConfig() const { return config; }

	//Operator
	//Calculates the luminance of a given image matrix
	static cv::Mat calculateLuminance(cv::Mat imageMatrix);

	//Operator method
	//Calculates the contrast ratio of two given regions of a luminance matrix
	static double ContrastBetweenRegions(const cv::Mat& luminance, const cv::Mat& maskA, const cv::Mat& maskB);

private:
	Configuration* config = nullptr;
	ITextboxDetection* textboxDetection = nullptr;
	ITextboxRecognition* textboxRecognition = nullptr;

	IChecker* contrastChecker = nullptr;
	IChecker* sizeChecker = nullptr;

	static std::vector<double>* linearizationLUT;
	static double linearize8bitRGB(const uchar& colorBits);

	FRIEND_TEST(LuminanceTests, MaxContrast);
	FRIEND_TEST(LuminanceTests, Commutative);
};

}