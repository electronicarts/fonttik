//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once

#include "fonttik/Frame.hpp"
#include "fonttik/TextBox.hpp"
#include "fonttik/Results.h"
#include <vector>

namespace tik 
{
class Configuration;

class IChecker 
{
public:
	virtual ~IChecker() { }

	virtual FrameResults check(const int& frameIndex, std::vector<TextBox>& boxes) = 0;
	virtual FrameResults check(const int& frameIndex, std::vector<TextBox>& boxes, std::vector<std::vector<TextBox>> colorblindBoxes) = 0;

protected:

	IChecker(Configuration* config) : configuration(config){};

	Configuration* configuration;
};

}
