#include "Image.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include "Media.h"

namespace fs = std::filesystem;

namespace tin {

	Image::Image(fs::path filePath, cv::Mat imgMat) :Media(filePath){
		imageMatrix = imgMat;
	}

	void Image::saveResultsOutlines(std::vector<std::vector<ResultBox>>& results, std::string fileName) {
		cv::Mat highlights = imageMatrix.clone();

		for (ResultBox& box : results.back()) {
			highlightBox(box.x, box.y, box.x + box.width, box.y + box.height, box.getResultColor(), highlights, 2);
		}
		saveOutputData(highlights, fileName+".png");
	}
}