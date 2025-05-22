//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include <string>

namespace tik {
	
class TextRecognitionParams;
class TextBox;

class ITextBoxRecognition {

public:
	virtual ~ITextBoxRecognition() {};

	virtual void init(const TextRecognitionParams& params) = 0;

	virtual std::string recognizeBox(TextBox& box) = 0;

protected:
	ITextBoxRecognition() {};
};

}