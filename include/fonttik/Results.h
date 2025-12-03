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

ResultType ResultTypeMerge(const ResultType a, const ResultType b);
std::string ResultTypeAsString(ResultType t);

struct ResultBox {
	ResultBox(ResultType type, int x, int y, int w, int h, double value) :
		type(type), x(x), y(y), width(w), height(h), value(value), text("") {};
	ResultBox(ResultType type, int x, int y, int w, int h, double value, const std::string& text) :
		type(type), x(x), y(y), width(w), height(h), value(value), text(text) {};
	ResultBox(ResultType type, cv::Rect r, double value) :
		type(type), x(r.x), y(r.y), width(r.width), height(r.height), value(value), text("") {};
	ResultBox(ResultType type, cv::Rect r, double value, const std::string& text) :
		type(type), x(r.x), y(r.y), width(r.width), height(r.height), value(value), text(text) {};

	ResultBox(ResultType type, cv::Rect r, double value, std::vector<double> colorblindValues, std::vector<ResultType> colorblindTypes) :
		type(type), x(r.x), y(r.y), width(r.width), height(r.height), value(value), text(""), colorblindValues(colorblindValues), colorblindTypes(colorblindTypes) {
	};
	
	ResultType type;
	int x, y, width, height;
	double value;
	std::string text;
	std::vector<double> colorblindValues = {};
	std::vector<ResultType> colorblindTypes = {};
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

	std::vector<ResultBox> results = {};
	ResultType overallType = ResultType::PASS;
	int frame;
	bool overallPass = true;
	std::vector<bool> overallColorblindPass = { true, true, true, true };
	std::vector<ResultType> overallColorblindType = { ResultType::PASS, ResultType::PASS, ResultType::PASS, ResultType::PASS };
};

class Results {
	
public:
	void clear() {
		overallContrastPass = true;
		overallSizePass = true;
		overallColorblindPass = { true, true, true, true };

		overallContrastResult = PASS;
		overallSizeResult = PASS;
		overallColorblindResult = { PASS, PASS, PASS, PASS };

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
		overallContrastResult = ResultTypeMerge(overallContrastResult,res.overallType);
		for (int i = 0; i < 4; i++) {
			overallColorblindPass[i] = overallColorblindPass[i] && res.overallColorblindPass[i];
			overallColorblindResult[i] = ResultTypeMerge(overallColorblindResult[i], res.overallColorblindType[i]);
		}
	}

	//Ads an already filled contrast results
	void addSizeResults(FrameResults res) {
		sizeResults.push_back(res);
		sortedSize = false;
		overallSizePass = overallSizePass && res.overallPass;
		overallSizeResult = ResultTypeMerge(overallSizeResult, res.overallType);
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
	ResultType overallContrastResult = PASS;
	bool overallSizePass = true;
	ResultType overallSizeResult = PASS;
	std::vector<bool> overallColorblindPass = { true, true, true, true }; //Protan, Deutan, Tritan, Grayscale
	std::vector<ResultType> overallColorblindResult = { PASS, PASS, PASS, PASS };
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