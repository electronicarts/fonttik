#include <iostream>
#include <fstream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>

//Original code from https://github.com/opencv/opencv/blob/master/samples/dnn/text_detection.cpp

void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result);

int main(int argc, char** argv)
{
    static const std::string kWinName = "EAST: An Efficient and Accurate Scene Text Detector";
    
    if (argc < 2) {
        std::cout << "Usage: \"./EAST image_path\"";
        return 1;
    }
    std::string imgPath(argv[1]);

	////This needs to be multiple of 32
	int inpWidth = 32 * (1920/32);
	int inpHeight = 32 * (1080 / 32);

	cv::dnn::TextDetectionModel_EAST east("frozen_east_text_detection.pb");
	east.setConfidenceThreshold(0.5);
	east.setNMSThreshold(0.4);

	// Parameters for Detection
	double detScale = 1.0;
	cv::Size detInputSize = cv::Size(inpWidth, inpHeight);
	cv::Scalar detMean = cv::Scalar(123.68, 116.78, 103.94);
	bool swapRB = true;
	east.setInputParams(detScale, detInputSize, detMean, swapRB);

	//Image reading
	cv::Mat img = cv::imread(imgPath);
	cv::resize(img, img, detInputSize);

	std::vector< std::vector<cv::Point> > detResults;
	east.detect(img, detResults);

	std::cout << "EAST found " << detResults.size() << "boxes\n";
	
    if (detResults.size() > 0) {
        // Text Recognition
        cv::Mat recInput = img;

        std::vector< std::vector<cv::Point> > contours;
        for (uint i = 0; i < detResults.size(); i++)
        {
            const auto& quadrangle = detResults[i];
            CV_CheckEQ(quadrangle.size(), (size_t)4, "");

            contours.emplace_back(quadrangle);

            std::vector<cv::Point2f> quadrangle_2f;
            for (int j = 0; j < 4; j++)
                quadrangle_2f.emplace_back(quadrangle[j]);

            cv::Mat cropped;
            fourPointsTransform(recInput, &quadrangle_2f[0], cropped);
        }
        polylines(img, contours, true, cv::Scalar(0, 255, 0), 2);
    }
    cv::imwrite(imgPath + "_results.png",img);
    cv::waitKey(0);

	std::cin.get();
}

void fourPointsTransform(const cv::Mat& frame, const cv::Point2f vertices[], cv::Mat& result)
{
    const cv::Size outputSize = cv::Size(100, 32);

    cv::Point2f targetVertices[4] = {
        cv::Point(0, outputSize.height - 1),
        cv::Point(0, 0), cv::Point(outputSize.width - 1, 0),
        cv::Point(outputSize.width - 1, outputSize.height - 1)
    };
    cv::Mat rotationMatrix = getPerspectiveTransform(vertices, targetVertices);

    warpPerspective(frame, result, rotationMatrix, outputSize);
}