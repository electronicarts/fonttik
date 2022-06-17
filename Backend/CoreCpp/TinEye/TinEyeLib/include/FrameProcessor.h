#pragma once
#include <thread>
#include "TinEye.h"
#include "Media.h"

namespace tin {
	class FrameProcessor {
		TinEye tineye;
	public:
		FrameProcessor() { std::cout << "Constructed FP" << std::endl; };
		virtual ~FrameProcessor() {
			std::cout << "Deleted FP" << std::endl;
			//if (tineye != nullptr) {
			//	delete tineye;
			//}
			//tineye = nullptr;
		}
		FrameProcessor(const FrameProcessor& source) {
		std::cout << "FP copy constructor" << std::endl;
		};
		void work(Media* media, std::mutex* mtx);
		void init(Configuration* config);
	};
}