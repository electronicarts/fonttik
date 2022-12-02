//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include "Textbox.h"

namespace tik {
	
class TextRecognitionParams;

class ITextboxRecognition {

public:
	virtual ~ITextboxRecognition() {};

	virtual void init(const TextRecognitionParams* params) = 0;

	virtual std::string recognizeBox(Textbox& box) = 0;

protected:
	ITextboxRecognition() {};
};

}