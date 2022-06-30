//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "FrameProcessor.h"

namespace tin {
	void FrameProcessor::init(Configuration* config) {
		tineye.init(config);
	}

	void FrameProcessor::work(Media* media, std::mutex* mtx) {
		mtx->lock();
		Frame* frame = media->getFrame();
		media->nextFrame();
		mtx->unlock();
		Results* mediaRes = media->getResultsPointer();
		while (frame!=nullptr) {
				std::pair<FrameResults, FrameResults> res = tineye.processFrame(frame);
				mediaRes->addSizeResults(res.first);
				mediaRes->addContrastResults(res.second);
				delete frame;
				mtx->lock();
				frame = media->getFrame();
				media->nextFrame();
				mtx->unlock();
		}
	}
}