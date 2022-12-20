//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "fonttik/Configuration.h"
#include "fonttik/Frame.h"
#include "fonttik/Textbox.h"
#include "fonttik/Results.h"
#include <vector>

namespace tik {

class IChecker {
public:
	virtual ~IChecker() { config = nullptr; }

	virtual FrameResults check(Frame& image, std::vector<Textbox>& boxes) = 0;

protected:
	Configuration* config = nullptr;

	IChecker(Configuration* config) : config(config) {};
};

}
