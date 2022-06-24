#pragma once
#include "ITextboxDetection.h"

namespace tin {
	class AppSettings;
	class TextDetectionParams;

	class TextboxDetectionEAST : public ITextboxDetection {
	protected:
		cv::dnn::TextDetectionModel_EAST* east;

		static void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);
	public:
		TextboxDetectionEAST() :ITextboxDetection() {};

		//Initialize textbox detection with configuration parameters, must be called before any detection calls
		virtual void init(const TextDetectionParams* params, const AppSettings* appSettingsCfg);

		//Releases memory used by textbox detection model
		virtual ~TextboxDetectionEAST();

		virtual std::vector<Textbox> detectBoxes(const cv::Mat& img);
	};

}