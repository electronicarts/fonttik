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

	struct FrameResults {
		int frame;
		bool overallPass = true;
		std::vector<ResultBox> results;

		constexpr bool operator <(const FrameResults& b) const {
			return frame < b.frame;
		};

		constexpr bool operator >(const FrameResults& b) const {
			return frame > b.frame;
		};

	};

	class Results {
		friend class FrameSorting;


		bool overallContrastPass = true;
		bool overallSizePass = true;
		bool warningsRaisedFlag = false;

		//Pairs' int value identifies the frame number of the result
		bool sortedContrast = false,
			sortedSize = false;
		std::vector<FrameResults> contrastResults;
		std::vector<FrameResults> sizeResults;

	public:
		void clear() {
			overallContrastPass = true;
			overallSizePass = true;
			warningsRaisedFlag = false;

			contrastResults.clear();
			sizeResults.clear();
		}
		bool contrastPass() const { return overallContrastPass; }
		bool sizePass() const { return overallSizePass; }
		bool warningsRaised() const { return warningsRaisedFlag; }
		void setContrastPass(bool to) { overallContrastPass = to; }
		void setSizePass(bool to) { overallSizePass = to; }
		void setWarningsRaised(bool to) { warningsRaisedFlag = to; }
		//Adds a frame with the specified ID and returns a reference to it
		FrameResults* addContrastResults(int frameID) {
			//TODO thread safety
			contrastResults.push_back({ frameID, {}});
			sortedContrast = false;
			return &contrastResults.back();
		}
		//Adds a frame with the specified ID and returns a reference to it
		FrameResults* addSizeResults(int frameID) {
			//TODO thread safety
			sizeResults.push_back({ frameID, {} });
			sortedSize = false;
			return &sizeResults.back();
		}

		std::vector<FrameResults>& getContrastResults() {
			if (!sortedContrast) {
				std::sort(contrastResults.begin(), contrastResults.end());
				sortedContrast = true;
			}
			return contrastResults;
		}

		std::vector<FrameResults>& getSizeResults() {
			if (!sortedSize) {
				std::sort(sizeResults.begin(), sizeResults.end());
				sortedSize= true;
			}
			return sizeResults;
		}

	};
}