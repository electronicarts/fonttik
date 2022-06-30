//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "IChecker.h"

namespace tin {
	class ContrastChecker : public IChecker {
	protected:
		bool textboxContrastCheck(Frame& image, Textbox& textbox, FrameResults& results);
	public:
		ContrastChecker(Configuration* config) : IChecker(config) {}

		virtual ~ContrastChecker() {}

		virtual FrameResults check(Frame& image, std::vector<Textbox>& boxes);
	};
}
