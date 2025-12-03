// Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.
#include "TextboxDetectionRekognition.h"
#include "fonttik/ConfigurationParams.hpp"
#include "fonttik/Log.h"



namespace tik 
{

void TextboxDetectionRekognition::init(const std::vector<double>& sRGB_LUT)
{
	//Store sRGB Look up table
	this->sRGB_LUT = sRGB_LUT;

#ifdef INIT_AWS
	options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
	options.loggingOptions.logger_create_fn = [] {
		return Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
			"console_logger", Aws::Utils::Logging::LogLevel::Info);
		};
	Aws::HttpOptions httpOptions{};
	httpOptions.installSigPipeHandler = true;
	options.httpOptions = httpOptions;
	Aws::InitAPI(options);
#endif
	Aws::Rekognition::RekognitionClientConfiguration config;
	client = new Aws::Rekognition::RekognitionClient(config);
};

TextboxDetectionRekognition::~TextboxDetectionRekognition() 
{
	if(client)
		delete client;

	client = nullptr;
#ifdef INIT_AWS
	Aws::ShutdownAPI(options);
#endif
}

std::vector<TextBox> TextboxDetectionRekognition::helperTextboxDetection(const cv::Mat& OGimg, Aws::Rekognition::Model::TextTypes type){
	cv::Mat img = OGimg.clone();
	std::vector<TextBox> boxes;
	while (true) {
		int count = 0;
		Aws::Rekognition::Model::DetectTextRequest request;
		std::vector<uchar> out;
		cv::Mat resizedImage;
		if (img.size().width > 1920 || img.size().height > 1080)
		{
			cv::resize(img, resizedImage, cv::Size(1920, 1080));
			//resizedImage = img;
		}
		else
		{
			resizedImage = img;
		}
		cv::imencode(".png", resizedImage, out);
		request.SetImage(Aws::Rekognition::Model::Image().WithBytes(Aws::Utils::ByteBuffer{ out.data(),out.size() }));

		std::vector< std::vector<cv::Point> > detResults;
		std::vector<std::string> texts;
		Aws::Rekognition::Model::DetectTextOutcome outcome;
		outcome = client->DetectText(request);
		if (!outcome.IsSuccess())
			throw outcome.GetError();

		auto  result = outcome.GetResult().GetTextDetections();
		for (auto res : result)
		{
			count++;

			auto& bb = res.GetGeometry().GetBoundingBox();
			int l = bb.GetLeft() * img.cols;
			int t = bb.GetTop() * img.rows;
			int w = bb.GetWidth() * img.cols;
			int h = bb.GetHeight() * img.rows;
			int offset = 1;

			auto bl = cv::Point{ l-offset, t + h + offset }; // bottom-left
			auto tl = cv::Point{ l-offset, t-offset }; // top-left
			auto tr = cv::Point{ l + w + offset, t-offset }; // top-right
			auto br = cv::Point{ l + w +offset, t + h +offset}; // bottom-right

			//Mask out this detected text
			cv::rectangle(img, cv::Rect(tl, br), cv::Scalar{ 0, 0, 0 }, -1);
			
			if (res.GetType() != type || (res.ConfidenceHasBeenSet() && res.GetConfidence() < 85))
				continue;
			detResults.push_back({ bl,tl,tr,br });
			texts.push_back(res.GetDetectedText());
		}

		for (int i = 0; i < detResults.size(); i++)
		{
			for (int j = 0; j < detResults[i].size(); j++)
			{

				//Make sure points are within bounds
				detResults[i][j].x = std::min(std::max(0, detResults[i][j].x), img.cols);
				detResults[i][j].y = std::min(std::max(0, detResults[i][j].y), img.rows);
			}
		}

		int i = 0;
		for (std::vector<cv::Point > points : detResults) {
			if (HorizontalTiltAngle(points[1], points[2]) < detectionParams->rotationThresholdRadians)
			{
				boxes.emplace_back(TextBox{ points, OGimg });
				boxes.back().setText(texts[i]);
				i++;
			}
			else
			{
				LOG_CORE_TRACE("Ignoring tilted text in {0}", points[1]);
			}
		}
		if (count < 100)
			break;
	}
	return boxes;
}


std::vector<TextBox> TextboxDetectionRekognition::detectBoxes(const cv::Mat& OGimg) 
{
	return helperTextboxDetection(OGimg);
}

LinesAndWords TextboxDetectionRekognition::detectLinesAndWords(const cv::Mat& OGimg)
{
	return
	{
		helperTextboxDetection(OGimg,Aws::Rekognition::Model::TextTypes::LINE),
		helperTextboxDetection(OGimg,Aws::Rekognition::Model::TextTypes::WORD)
	};
}

}