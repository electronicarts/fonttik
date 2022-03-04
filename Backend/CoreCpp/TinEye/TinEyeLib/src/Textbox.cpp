#include "Textbox.h"

Textbox::Textbox(const std::vector<cv::Point >& points, int padding) : parentImage(nullptr) {
	//Height takes into account possible box skewing when calculating due to letters going down (eg p's)
	int boxHeight = std::max(points[0].y, points[3].y)-points[1].y;
	int boxWidth = points[3].x - points[1].x;

	textboxRect = cv::Rect(points[1].x, points[1].y, boxWidth, boxHeight);
}

Textbox::Textbox(cv::Rect rect):parentImage(nullptr) {
	textboxRect = rect;
}

void Textbox::setParentImage(Image* img) {
	parentImage = img;
	submatrix = parentImage->getImageMatrix()(textboxRect);
}

cv::Mat Textbox::getLuminanceHistogram() {
	if (luminanceHistogram.empty()) {
		luminanceHistogram = parentImage->calculateLuminanceHistogram(textboxRect);
	}

	return luminanceHistogram;
}

cv::Mat Textbox::getSurroundingLuminanceHistogram(int marginX, int marginY)
{
	//Make sure the surrounding bounding box is inside the original matrix's bounds
	cv::Mat mat = parentImage->getImageMatrix();
	int
		marginLeft = (textboxRect.x-marginX >= 0) ? marginX : textboxRect.x,
		marginRight = (textboxRect.x + textboxRect.width + marginX < mat.cols) ? marginX : mat.cols - textboxRect.width - textboxRect.x,
		marginTop = (textboxRect.y - marginY >= 0) ? marginY : textboxRect.y,
		marginBottom = (textboxRect.y + textboxRect.height + marginY < mat.rows) ? marginY : mat.rows - textboxRect.height - textboxRect.y;
	return parentImage->calculateLuminanceHistogram(cv::Rect(textboxRect.x-marginLeft,textboxRect.y - marginTop,
		textboxRect.width + marginLeft + marginRight,textboxRect.height + marginTop + marginBottom),
		cv::Rect(marginLeft, marginRight, textboxRect.width, textboxRect.height));
}

std::pair<float,float> Textbox::OverlapAxisPercentage(const Textbox& a, const Textbox& b) {
	cv::Rect aRect = a.textboxRect, bRect = b.textboxRect;
	cv::Rect overlap = aRect & bRect;
	float xOverlap = static_cast<float>(overlap.width) / std::min(aRect.width, bRect.width);
	float yOverlap = static_cast<float>(overlap.height) / std::min(aRect.height, bRect.height);

	return { xOverlap,yOverlap };
}