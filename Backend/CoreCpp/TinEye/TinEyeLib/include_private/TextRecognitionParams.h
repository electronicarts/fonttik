#pragma once
#include <array>
#include <string>

namespace tin {
	class TextRecognitionParams {
	private:
		std::string recognitionModel;
		std::string decodeType;
		std::string vocabularyFilepath;

	public:
		TextRecognitionParams() {}
		TextRecognitionParams(std::string model, std::string decode, std::string vocabularyFile) :
			recognitionModel(model), decodeType(decode), vocabularyFilepath(vocabularyFile) {};

		std::string getRecognitionModel() const { return recognitionModel; }
		std::string getDecodeType() const { return decodeType; }
		std::string getVocabularyFilepath() const { return vocabularyFilepath; }
	};
}