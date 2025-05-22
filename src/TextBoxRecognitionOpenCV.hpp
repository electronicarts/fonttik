//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "ITextboxRecognition.h"
#include <opencv2/dnn.hpp>
#include "fonttik/ConfigurationParams.hpp"


namespace tik 
{

class TextBox;

class TextBoxRecognitionOpenCV : public ITextBoxRecognition 
{

public:
	TextBoxRecognitionOpenCV() :ITextBoxRecognition() {};

	virtual ~TextBoxRecognitionOpenCV() {}

	virtual void init(const TextRecognitionParams& params) override;

	virtual std::string recognizeBox(TextBox& box);

protected:
	cv::dnn::TextRecognitionModel textRecognition;

};

}