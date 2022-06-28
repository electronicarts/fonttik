#pragma once
#include <vector>
#include "Textbox.h"

namespace tin {
	class TextRecognitionParams;

	class ITextboxRecognition {
	protected:
		ITextboxRecognition() {};
	public:
		virtual ~ITextboxRecognition() {};

		virtual void init(const TextRecognitionParams* params) = 0;

		virtual std::string recognizeBox(Textbox& box) = 0;
	};
}