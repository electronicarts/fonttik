#include <thread>
#include "TinEye.h"
#include "Media.h"

namespace tin {
	class FrameProcessingThread {
		TinEye tineye;
	public:
		FrameProcessingThread(Configuration* config);
		void work(Media* media);
	};
}