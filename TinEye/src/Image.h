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

	cv::Mat getLuminanceMap();
	void saveLuminanceMap(std::string filepath);
};