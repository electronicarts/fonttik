#include "benchmark/benchmark.h"
#include "TinEye.h"
#include "Configuration.h"
#include "Media.h"



static void BM_EASTDetection(benchmark::State& state) {
	cv::Mat img;
	cv::dnn::TextDetectionModel_EAST* east;
	//per-thread setup

	east = new cv::dnn::TextDetectionModel_EAST("frozen_east_text_detection.pb");

	//Confidence on textbox threshold
	east->setConfidenceThreshold(0.5);
	//Non Maximum supression
	east->setNMSThreshold(0.4);

	east->setInputScale(1.0);

	cv::Scalar detMean(123.68, 116.78, 103.94);
	east->setInputMean(detMean);

	east->setInputSwapRB(true);

	img = cv::imread("resources/knockout/Knockout1080.bmp", cv::IMREAD_COLOR);
	for (auto _ : state) {
		//Calculate needed conversion for new width and height to be multiples of 32
		////This needs to be multiple of 32
		int inpWidth = 32 * (img.cols / 32 + ((img.cols % 32 != 0) ? 1 : 0));
		int inpHeight = 32 * (img.rows / 32 + ((img.rows % 32 != 0) ? 1 : 0));
		float widthRatio = float(inpWidth) / img.cols;
		float heightRatio = float(inpHeight) / img.rows;

		cv::Size detInputSize = cv::Size(inpWidth, inpHeight);

		east->setInputSize(detInputSize);

		//Image reading
		cv::Mat resizedImg;
		cv::resize(img, resizedImg, detInputSize);

		std::vector< std::vector<cv::Point> > detResults;
		{
			east->detect(resizedImg, detResults);
		}
	}

	delete east;
}

BENCHMARK(BM_EASTDetection)->Iterations(5)->Unit(benchmark::kSecond)->ThreadRange(1, 8);

static void BM_TextRecognition(benchmark::State& state) {
	cv::Mat img;
	cv::dnn::TextRecognitionModel textRecognition = cv::dnn::TextRecognitionModel("crnn_cs.onnx");
	textRecognition.setDecodeType("CTC-greedy");
	std::ifstream vocFile;
	vocFile.open("alphabet_94.txt");
	CV_Assert(vocFile.is_open());
	std::string vocLine;
	std::vector<std::string> vocabulary;
	while (std::getline(vocFile, vocLine)) {
		vocabulary.push_back(vocLine);
	}
	textRecognition.setVocabulary(vocabulary);

	// Normalization parameters
	auto mean = cv::Scalar(127.5, 127.5, 127.5);
	// The input shape
	std::pair<int, int> size = { 100,32 };
	textRecognition.setInputParams(1.0/127.5, cv::Size(size.first, size.second), cv::Scalar(mean[0], mean[1], mean[2]));

	img = cv::imread("resources/knockout/Knockout1080.bmp", cv::IMREAD_COLOR);
	for (auto _ : state) {
		textRecognition.recognize(img);
	}
}

BENCHMARK(BM_TextRecognition)->Iterations(5)->Unit(benchmark::kSecond)->ThreadRange(1, 8);

BENCHMARK_MAIN();