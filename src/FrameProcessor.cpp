//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "fonttik/FrameProcessor.h"

namespace tik {
	void FrameProcessor::init(Configuration* config) {
		fonttik.init(config);
	}

	void FrameProcessor::work(Media* media, std::mutex* mtx) {
		//Lock mutex to load next frame
		mtx->lock();
		Frame* frame = media->getFrame();
		media->nextFrame();
		mtx->unlock();

		Results* mediaRes = media->getResultsPointer();
		while (frame!=nullptr) {
				std::pair<FrameResults, FrameResults> res = fonttik.processFrame(frame);
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