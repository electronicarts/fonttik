#include "TextboxRecognitionOpenCV.h"
#include "TextRecognitionParams.h"
#include <fstream>

namespace tin {
	void TextboxRecognitionOpenCV::init(const TextRecognitionParams* params) {
		textRecognition = cv::dnn::TextRecognitionModel(params->getRecognitionModel());
		textRecognition.setDecodeType(params->getDecodeType());
		std::ifstream vocFile;
		vocFile.open(params->getVocabularyFilepath());
		CV_Assert(vocFile.is_open());
		std::string vocLine;
		std::vector<std::string> vocabulary;
		while (std::getline(vocFile, vocLine)) {
			vocabulary.push_back(vocLine);
		}
		textRecognition.setVocabulary(vocabulary);

		// Normalization parameters
		auto mean = params->getMean();
		// The input shape
		std::pair<int, int> size = params->getSize();
		textRecognition.setInputParams(params->getScale(), cv::Size(size.first, size.second), cv::Scalar(mean[0], mean[1], mean[2]));

		textRecognition.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
		textRecognition.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	}

	std::string TextboxRecognitionOpenCV::recognizeBox(Textbox& box) {
		return textRecognition.recognize(box.getSubmatrix());
	}
}