// Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include "ITextboxDetection.h"
#include "aws/rekognition/RekognitionServiceClientModel.h"
#include "aws/rekognition/RekognitionClient.h"
#include <aws/rekognition/model/DetectTextRequest.h>
#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include "TextboxDetectionRekognition.h"

namespace tik 
{

class TextDetectionParams;

class TextboxDetectionRekognition : public ITextboxDetection {

public:
	TextboxDetectionRekognition(const TextDetectionParams& params) : ITextboxDetection(params) {}
	~TextboxDetectionRekognition() override;

	////Initialize textbox detection with configuration parameters, must be called before any detection calls
	virtual void init(const std::vector<double>& sRGB_LUT);

	virtual std::vector<TextBox> detectBoxes(const cv::Mat& img);
	virtual LinesAndWords detectLinesAndWords(const cv::Mat& img);

private:
	std::vector<TextBox>helperTextboxDetection(const cv::Mat& OGimg, Aws::Rekognition::Model::TextTypes type = Aws::Rekognition::Model::TextTypes::WORD);

	Aws::SDKOptions options;
	Aws::Rekognition::RekognitionClient* client;
};

}
