#pragma once
#include "ITextboxRecognition.h"
#include <opencv2/dnn.hpp>

namespace tin {
	class TextboxRecognitionOpenCV : public ITextboxRecognition {
	protected:
		cv::dnn::TextRecognitionModel textRecognition;

	public:
		TextboxRecognitionOpenCV() :ITextboxRecognition() {};

		virtual ~TextboxRecognitionOpenCV() {}

		virtual void init(const TextRecognitionParams* params);

		virtual std::string recognizeBox(Textbox& box);
	};

}