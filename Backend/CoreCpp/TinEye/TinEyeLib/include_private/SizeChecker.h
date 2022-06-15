#pragma once
#include "IChecker.h"
#include "ITextboxRecognition.h"

namespace tin {
	class SizeChecker : public IChecker {
	protected:
		ITextboxRecognition* textboxRecognition = nullptr;

		bool textboxSizeCheck(Frame& image, Textbox& textbox, FrameResults& results);
	public:
		SizeChecker(Configuration* config, ITextboxRecognition* textboxRecognition) : IChecker(config), textboxRecognition(textboxRecognition) {}

		virtual ~SizeChecker() { textboxRecognition = nullptr; }

		virtual FrameResults check(Frame& image, std::vector<Textbox>& boxes);
	};
}
