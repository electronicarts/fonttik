//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "IChecker.h"

namespace tik {
	
class ContrastChecker : public IChecker {

public:
	ContrastChecker(Configuration* config) : IChecker(config) {}

	virtual ~ContrastChecker() {}

	virtual FrameResults check(Frame& image, std::vector<Textbox>& boxes);

protected:
	bool textboxContrastCheck(Frame& image, Textbox& textbox, FrameResults& results);
};

}
