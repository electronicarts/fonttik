// Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.
#include "TextboxDetectionDB.h"
#include "TextDetectionParams.h"
#include "AppSettings.h"
#include "Instrumentor.h"
#include "Log.h"

namespace tin {
	void TextboxDetectionDB::init(const TextDetectionParams* params, const AppSettings* appSettingsCfg) {
		detectionParams = params;

		const DBDetectionParams* dbParams = params->getDBParams();
		
		//Models can be found in https://github.com/opencv/opencv/blob/master/doc/tutorials/dnn/dnn_text_spotting/dnn_text_spotting.markdown
		db = new cv::dnn::TextDetectionModel_DB(dbParams->getDetectionModel());

		// Post-processing parameters
		db->setBinaryThreshold(dbParams->getBinaryThreshold())
			.setPolygonThreshold(dbParams->getPolygonThreshold())
			.setMaxCandidates(dbParams->getMaxCandidates())
			.setUnclipRatio(dbParams->getUnclipRatio())
			;

		// Normalization parameters
		//Not the same as in EASt
		double scale = dbParams->getScale();
		//Default values from documentation are (123.68, 116.78, 103.94);
		auto mean = dbParams->getMean();
		cv::Scalar detMean(mean[0], mean[1], mean[2]);

		// The input shape
		auto size = dbParams->getInputSize();
		cv::Size inputSize = cv::Size(736, 736);

		db->setInputParams(scale, inputSize, detMean);

		db->setPreferableBackend((cv::dnn::Backend) dbParams->getPreferredBackend());
		db->setPreferableTarget((cv::dnn::Target) dbParams->getPreferredTarget());
	}

	TextboxDetectionDB::~TextboxDetectionDB() {
		if (db != nullptr) {
			delete db;
		}
		db = nullptr;
	} 

	std::vector<Textbox> TextboxDetectionDB::detectBoxes(const cv::Mat& img) {

		std::vector< std::vector<cv::Point> > detResults;
		{
			PROFILE_SCOPE("DB DNN");
			db->detect(img, detResults);
		}

		std::vector<Textbox> boxes;
		for (std::vector<cv::Point > points : detResults) {
			if (HorizontalTiltAngle(points[1], points[2]) < detectionParams->getRotationThresholdRadians()) {
				boxes.emplace_back(points);
			}
			else {
				LOG_CORE_TRACE("Ignoring tilted text in {0}", points[1]);
			}
		}

		for (int i = 0; i < detResults.size(); i++) {
			for (int j = 0; j < detResults[i].size(); j++) {

				//Make sure points are within bounds
				detResults[i][j].x = std::min(std::max(0, detResults[i][j].x), img.cols);
				detResults[i][j].y = std::min(std::max(0, detResults[i][j].y), img.rows);
			}
		}

		//Points are
		/*
		[1]---------[2]
		|            |
		|            |
		[0]---------[3]
		*/

		return boxes;
	}
}