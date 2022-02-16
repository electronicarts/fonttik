#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

class Image {
protected:
	cv::Mat imageMatrix;
	cv::Mat luminanceMap;

	float linearize8bitRGB(const uchar& colorBits);

	
public:
	Image();

	bool loadImage(std::string filepath);

	//Returns loaded image matrix
	cv::Mat getImageMatrix();

	//Returns loaded image's luminance map, if map hasn't been calculated calculates it as well
	cv::Mat getLuminanceMap();

	//Saves calculated luminance map to specified filepath
	void saveLuminanceMap(std::string filepath);

	//Flips the luminance of a given region
	void flipLuminance(const int& x1, const int& y1, const int& x2, const int& y2);

	//Flips the luminance of the whole image
	void flipLuminance();

	//Returns average surrounding luminance of a given bounding box in luminance map
	uchar getAverageSurroundingLuminance(const int& x1, const int& y1, const int& x2, const int& y2, const int& marginX = 3, const int& marginY = 3);

	//Hightlights box in specified matrix
	static void highlightBox(const int& x1, const int& y1, const int& x2, const int& y2, cv::Scalar& color, cv::Mat& matrix);
};