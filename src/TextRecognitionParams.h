//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <string>
#include <array>
#include <nlohmann/json.hpp>

namespace tik {
	
class TextRecognitionParams {
public:
	TextRecognitionParams() : recognitionModel("crnn_cs.onnx"), decodeType("CTC-greedy"),
	vocabularyFilepath("alphabet_94.txt"), scale(1.0 / 127.5), mean({ 127.5 ,127.5 ,127.5 }),
	size({ 100,32 }) {}

	void init(nlohmann::json textRecognition) {
		nlohmann::json scale = textRecognition["scale"];
		nlohmann::json jmean = textRecognition["mean"];
		nlohmann::json jsize = textRecognition["inputSize"];
			
		recognitionModel = textRecognition["recognitionModel"];
		decodeType = textRecognition["decodeType"];
		vocabularyFilepath = textRecognition["vocabularyFile"];
		scale = (double)scale["numerator"] / (double)scale["denominator"];
		mean = { jmean[0],jmean[1] ,jmean[2] };
		size = { jsize["width"], jsize["height"] };
	}

	std::string getRecognitionModel() const { return recognitionModel; }
	std::string getDecodeType() const { return decodeType; }
	std::string getVocabularyFilepath() const { return vocabularyFilepath; }
	double getScale() const { return scale; }
	std::array<double, 3> getMean() const { return mean; }
	std::pair<int, int> getSize() const { return size; }

private:
	std::string recognitionModel;
	std::string decodeType;
	std::string vocabularyFilepath;
	double scale;
	std::array<double, 3> mean;
	std::pair<int, int> size;

};

}