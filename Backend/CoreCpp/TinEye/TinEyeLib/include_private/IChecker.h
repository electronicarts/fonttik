#pragma once
#include "Configuration.h"
#include "Media.h"
#include "Textbox.h"
#include <vector>

namespace tin {

	class IChecker {
	protected:
		Configuration* config = nullptr;

		IChecker(Configuration* config) : config(config) {};
	public:
		virtual ~IChecker() { config = nullptr; }

		virtual bool check(Media& image, std::vector<Textbox>& boxes) = 0;
	};

}
