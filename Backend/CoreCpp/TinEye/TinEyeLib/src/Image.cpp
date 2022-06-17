#include "Image.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include "Media.h"
#include "Frame.h"

namespace fs = std::filesystem;

namespace tin {

	Image::Image(fs::path filePath, cv::Mat imgMat) :Media(filePath) {
		imageMatrix = imgMat;
	}

	void Image::saveResultsOutlines(std::vector<FrameResults>& results, fs::path path, bool saveNumbers) {
		cv::Mat highlights = imageMatrix.clone();

		for (ResultBox& box : results.back().results) {
			cv::Scalar color = box.getResultColor();
			Frame::highlightBox(box.x, box.y, box.x + box.width, box.y + box.height, color, highlights, 2);
		}

		//Add measurements after boxes so boxes don't cover the numbers
		if (saveNumbers) {
			for (ResultBox& box : results.back().results) {
				Frame::putResultBoxValues(highlights, box, (path.stem() == "contrastChecks") ? 1 : 0); //Only add decimals with contrast checks
			}
		}

		saveOutputData(highlights, path.string() + ".png");
	}

	Frame* Image::getFrame() {
		Frame* frame = (!processed) ? new Frame(this,0,imageMatrix) : nullptr;
		processed = true;

		return frame;
	}
}