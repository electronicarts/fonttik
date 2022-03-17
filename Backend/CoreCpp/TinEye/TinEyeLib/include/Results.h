#pragma once
#include <vector>

namespace tin {
	enum ResultType
	{
		PASS,
		FAIL,
		WARNING,
		UNRECOGNIZED
	};

	struct ResultBox {
		ResultType type;
		int x, y, width, height;

		ResultBox(ResultType type, int x, int y, int w, int h) :
			type(type), x(x), y(y), width(w), height(h) {}
	};

	struct Results {
		bool overallContrastPass = true;
		bool overallSizePass = true;
		bool warningsRaised = false;

		std::vector<std::vector<ResultBox>> contrastResults;
		std::vector < std::vector<ResultBox>> sizeResults;

		void clear() {
			overallContrastPass = true;
			overallSizePass = true;
			warningsRaised = false;

			contrastResults.clear();
			sizeResults.clear();
		}
	};
}