//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <thread>
#include "fonttik/Fonttik.h"
#include "fonttik/Media.h"

namespace tik {

class FrameProcessor {
public:
	FrameProcessor() {};
	virtual ~FrameProcessor() {		};
	FrameProcessor(const FrameProcessor& source) {
		//Can't copy Fonttik, two instances cant access the same text detector or OCR, we need to create a new one with the same config;
		auto cfg = source.fonttik.getConfig();
		if (cfg != nullptr) {
			init(cfg);
		}
	};
	void work(Media* media, std::mutex* mtx);
	void init(Configuration* config);

private:
	Fonttik fonttik;
};

}