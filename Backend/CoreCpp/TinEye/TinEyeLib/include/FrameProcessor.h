#pragma once
#include <thread>
#include "TinEye.h"
#include "Media.h"

namespace tin {
	class FrameProcessor {
		TinEye tineye;
	public:
		FrameProcessor() {};
		virtual ~FrameProcessor() {		};
		FrameProcessor(const FrameProcessor& source) {
			//Can't copy TinEye, two instances cant access the same text detector or OCR, we need to create a new one with the same config;
			auto cfg = source.tineye.getConfig();
			if (cfg != nullptr) {
				init(cfg);
			}
		};
		void work(Media* media, std::mutex* mtx);
		void init(Configuration* config);
	};
}