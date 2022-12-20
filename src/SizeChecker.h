//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "IChecker.h"
#include "ITextboxRecognition.h"

namespace tik {

class SizeChecker : public IChecker {
public:
	SizeChecker(Configuration* config, ITextboxRecognition* textboxRecognition) : IChecker(config), textboxRecognition(textboxRecognition) {}

	virtual ~SizeChecker() { textboxRecognition = nullptr; }

	virtual FrameResults check(Frame& image, std::vector<Textbox>& boxes);

protected:
	ITextboxRecognition* textboxRecognition = nullptr;

	bool textboxSizeCheck(Frame& image, Textbox& textbox, FrameResults& results);

};

}
