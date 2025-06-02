//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#include "TextBoxRecognitionOpenCV.hpp"
#include "fonttik/TextBox.hpp"
#include <fstream>


namespace tik 
{

void TextBoxRecognitionOpenCV::init(const TextRecognitionParams& params) 
{
	textRecognition = cv::dnn::TextRecognitionModel(params.recognitionModel);
	textRecognition.setDecodeType(params.decodeType);

	std::ifstream vocFile;
	vocFile.open(params.vocabularyFilePath);
	CV_Assert(vocFile.is_open());
	std::string vocLine;
	std::vector<std::string> vocabulary;
	
	while (std::getline(vocFile, vocLine)) 
	{
		vocabulary.push_back(vocLine);
	}
	
	textRecognition.setVocabulary(vocabulary);

	// Normalization parameters
	auto mean = params.mean;
	// The input shape
	std::pair<int, int> size = params.size;
	textRecognition.setInputParams(params.scale, cv::Size(size.first, size.second), cv::Scalar(mean[0], mean[1], mean[2]));

	textRecognition.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	textRecognition.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
}

std::string TextBoxRecognitionOpenCV::recognizeBox(TextBox& box) 
{
	return textRecognition.recognize(box.getSubMatrix());
}

}