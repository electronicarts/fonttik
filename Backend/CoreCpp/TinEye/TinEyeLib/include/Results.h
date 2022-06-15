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
		std::vector<ResultBox> results = {};

		constexpr bool operator <(const FrameResults& b) const {
			return frame < b.frame;
		};

		constexpr bool operator >(const FrameResults& b) const {
			return frame > b.frame;
		};

		FrameResults(int frameID) :frame(frameID) {};

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
		std::mutex contrast_mtx;
		std::vector<FrameResults> sizeResults;
		std::mutex size_mtx;

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
		
		//Ads an already filled contrast results
		void addContrastResults(FrameResults res) {
			contrast_mtx.lock();
			
			contrastResults.push_back(res);
			sortedContrast = false;
			overallContrastPass = overallContrastPass && res.overallPass;

			contrast_mtx.unlock();
		}

		//Ads an already filled contrast results
		void addSizeResults(FrameResults res) {
			size_mtx.lock();
			
			sizeResults.push_back(res);
			sortedSize = false;
			overallSizePass = overallSizePass && res.overallPass;
			
			size_mtx.unlock();
		}

		std::vector<FrameResults>& getContrastResults() {
			contrast_mtx.lock();

			if (!sortedContrast) {
				std::sort(contrastResults.begin(), contrastResults.end());
				sortedContrast = true;
			}
			std::vector<FrameResults>& res = contrastResults;
			
			contrast_mtx.unlock();
			return res;
		}

		std::vector<FrameResults>& getSizeResults() {
			size_mtx.lock();

			if (!sortedSize) {
				std::sort(sizeResults.begin(), sizeResults.end());
				sortedSize= true;
			}
			std::vector<FrameResults>& res = sizeResults;
			
			size_mtx.unlock();
			return sizeResults;
		}

	};
}