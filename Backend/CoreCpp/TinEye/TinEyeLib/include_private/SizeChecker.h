#pragma once
#include "IChecker.h"
#include "ITextboxRecognition.h"

namespace tin {
	class SizeChecker : public IChecker {
	protected:
		ITextboxRecognition* textboxRecognition = nullptr;

		bool textboxSizeCheck(Media& image, Textbox& textbox);
	public:
		SizeChecker(Configuration* config, ITextboxRecognition* textboxRecognition) : IChecker(config), textboxRecognition(textboxRecognition) {}

		virtual ~SizeChecker() { textboxRecognition = nullptr; }

		virtual bool check(Media& image, std::vector<Textbox>& boxes);
	};
}
