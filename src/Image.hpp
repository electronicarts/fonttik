//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "fonttik/Media.hpp"


namespace tik
{

class Image : public Media
{
public:
	Image(std::string mediaSource, cv::Mat img, ColorblindFilters* colorblindFilters);
	virtual ~Image() = default;


	/// <summary>
	/// Will only be performed once to analyse the image
	/// </summary>
	/// <returns>True on first call and false once the image has been processed</returns>
	virtual bool loadFrame() override;

	/// <summary>
	/// Return Frame object with the loaded image mat
	/// </summary>
	virtual Frame getFrame() override;
	virtual std::vector<Frame> getColorblindFrames() override;

	std::pair<fs::path, fs::path>saveResultsOutlines(const SaveResultProperties& sizeResultProperties, 
		const SaveResultProperties& contrastResultProperties) override;

	void saveResultsOutlinesAsync(const SaveResultProperties& sizeResultProperties, 
		const SaveResultProperties& contrastResultProperties, rigtorp::SPSCQueue<FrameResult>& queue, std::atomic<bool>& done) override;

	virtual std::string getExtension() override { return ".png"; }

	virtual void calculateMask(const MaskParams& params) override;

private:
	fs::path saveResultsOutlines(const std::vector<FrameResults>& results, fs::path path, 
		const std::vector<cv::Scalar>& colors, bool saveNumbers);

	void storeResultsInJSON(const std::vector<ResultBox>& res, int id, std::ofstream& out, bool contrast=false);

	void saveColorblindImages();

	Frame frame;
	Frame protanFrame, deutanFrame, tritanFrame, grayscaleFrame;
	bool processed;
	
	
};

}