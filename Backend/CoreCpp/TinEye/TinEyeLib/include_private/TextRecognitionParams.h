#pragma once
#include <string>
#include <array>

namespace tin {
	class TextRecognitionParams {
	private:
		std::string recognitionModel;
		std::string decodeType;
		std::string vocabularyFilepath;
		double scale;
		std::array<double, 3> mean;
		std::pair<int, int> size;

	public:
		TextRecognitionParams() {}
		TextRecognitionParams(std::string model, std::string decode, std::string vocabularyFile,
			double scale, std::array<double, 3> mean, std::pair<int, int> size) :
			recognitionModel(model), decodeType(decode), vocabularyFilepath(vocabularyFile),
			scale(scale), mean(mean), size(size) {};

		std::string getRecognitionModel() const { return recognitionModel; }
		std::string getDecodeType() const { return decodeType; }
		std::string getVocabularyFilepath() const { return vocabularyFilepath; }
		double getScale() const { return scale; }
		std::array<double, 3> getMean() const { return mean; }
		std::pair<int, int> getSize() const { return size; }
	};
}