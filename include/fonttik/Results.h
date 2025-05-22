//Copyright (C) 2022-2025 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

namespace tik {

enum ResultType
{
	PASS = 0,
	FAIL,
	WARNING,
	UNRECOGNIZED,
	//Not an actual result, but usefull to know how many types of resutls there are 
	//Keep always as the last item
	RESULTYPE_COUNT
};

std::string ResultTypeAsString(ResultType t);

struct ResultBox {
	ResultBox(ResultType type, int x, int y, int w, int h, double value) :
		type(type), x(x), y(y), width(w), height(h), value(value), text("") {};
	ResultBox(ResultType type, int x, int y, int w, int h, double value, const std::string& text) :
		type(type), x(x), y(y), width(w), height(h), value(value), text(text) {};
	
	ResultType type;
	int x, y, width, height;
	double value;
	std::string text;
};

struct FrameResults 
{
	FrameResults(int frameID) :frame(frameID) {};
	
	constexpr bool operator <(const FrameResults& b) const {
		return frame < b.frame;
	};

	constexpr bool operator >(const FrameResults& b) const {
		return frame > b.frame;
	};

	int frame;
	bool overallPass = true;
	std::vector<ResultBox> results = {};

};

class Results {
	
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
		contrastResults.push_back(res);
		sortedContrast = false;
		overallContrastPass = overallContrastPass && res.overallPass;
	}

	//Ads an already filled contrast results
	void addSizeResults(FrameResults res) {
		sizeResults.push_back(res);
		sortedSize = false;
		overallSizePass = overallSizePass && res.overallPass;
	}

	/// <summary>
	/// Returns the sorted contrast resutls
	/// </summary>
	/// <returns></returns>
	std::vector<FrameResults>& getContrastResults() {
		//Data is only sorted upon retrieval
		if (!sortedContrast) {
			std::sort(contrastResults.begin(), contrastResults.end());
			sortedContrast = true;
		}
		std::vector<FrameResults>& res = contrastResults;
		return res;
	}
		
	/// <summary>
	/// /// Returns the sorted size resutls
	/// </summary>
	/// <returns></returns>
	std::vector<FrameResults>& getSizeResults() {
		//Data is only sorted upon retrieval
		if (!sortedSize) {
			std::sort(sizeResults.begin(), sizeResults.end());
			sortedSize= true;
		}
		std::vector<FrameResults>& res = sizeResults;
		return sizeResults;
	}

private:
	friend class FrameSorting; //Unit testing class


	bool overallContrastPass = true;
	bool overallSizePass = true;
	bool warningsRaisedFlag = false;

	// Flags to keep track of the sorted status of the results
	// Flase: a result has been added and we can't ensure they are sorted
	// True: we can ensure the results are sorted
	bool sortedContrast = false,
		sortedSize = false;

	//both mutex allow for different threads saving their results in any order

	std::vector<FrameResults> contrastResults;
	std::vector<FrameResults> sizeResults;
};

}