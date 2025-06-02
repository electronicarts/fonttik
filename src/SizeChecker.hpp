//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "IChecker.h"
#include "ITextboxRecognition.h"

namespace tik
{
class TextBox;
class TextSizeParams;
class Configuration;

class SizeChecker : public IChecker
{
public:
	SizeChecker(Configuration* config, ITextBoxRecognition* textboxRecognition);
	virtual ~SizeChecker() { textboxRecognition = nullptr; }

	virtual FrameResults check(const int& frameIndex, std::vector<TextBox>& textBoxes) override;

protected:
	bool textBoxSizeCheck(TextBox& textBox, FrameResults& results);
	
	void recognizeText(std::string& recognitionResult, TextBox& textBox);

	ITextBoxRecognition* textboxRecognition = nullptr;

};

}
