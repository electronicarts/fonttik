#pragma once
#include "IChecker.h"

namespace tin {
	class ContrastChecker : public IChecker {
	protected:
		bool textboxContrastCheck(Media& image, Textbox& textbox);
	public:
		ContrastChecker(Configuration* config) : IChecker(config) {}

		virtual ~ContrastChecker() {}

		virtual bool check(Media& image, std::vector<Textbox>& boxes);
	};
}
