#include <vector>
#include <opencv2/core.hpp>

class TextboxDetection {
protected:
	static void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);
public:
	static std::vector< std::vector<cv::Point> > detectBoxes(cv::Mat img,bool debug=false);
};