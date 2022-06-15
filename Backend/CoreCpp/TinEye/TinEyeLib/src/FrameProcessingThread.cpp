#include "FrameProcessingThread.h"

namespace tin {
	FrameProcessingThread::FrameProcessingThread(Configuration* config):tineye(config) {}

	void FrameProcessingThread::work(Media* media) {
		Frame* frame = media->getFrame();
		Results* mediaRes = media->getResultsPointer();
		while (frame!=nullptr) {
				std::pair<FrameResults, FrameResults> res = tineye.processFrame(frame);
				mediaRes->addSizeResults(res.first);
				mediaRes->addContrastResults(res.second);
				delete frame;
				media->nextFrame();
				frame = media->getFrame();
		}
	}
}