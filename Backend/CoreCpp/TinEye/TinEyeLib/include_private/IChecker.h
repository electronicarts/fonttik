//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "Configuration.h"
#include "Frame.h"
#include "Textbox.h"
#include "Results.h"
#include <vector>

namespace tin {

	class IChecker {
	protected:
		Configuration* config = nullptr;

		IChecker(Configuration* config) : config(config) {};
	public:
		virtual ~IChecker() { config = nullptr; }

		virtual FrameResults check(Frame& image, std::vector<Textbox>& boxes) = 0;
	};

}
