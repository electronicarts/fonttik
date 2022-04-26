#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

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
		double value;

		ResultBox(ResultType type, int x, int y, int w, int h, double value) :
			type(type), x(x), y(y), width(w), height(h), value(value) {}

		cv::Scalar getResultColor() {
			cv::Scalar color;
			switch (type) {
			case ResultType::PASS:
				color = cv::Scalar(0, 255, 0);
				break;
			case ResultType::FAIL:
				color = cv::Scalar(0, 0, 255);
				break;
			case ResultType::UNRECOGNIZED:
				color = cv::Scalar(255, 0, 0);
				break;
			case ResultType::WARNING:
				color = cv::Scalar(0, 170, 255);
				break;
			}
			return color;
		}
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