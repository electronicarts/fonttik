////Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.
//
//#include <gtest/gtest.h>
//#include "fonttik/TextBox.hpp"
//#include "../src/TextboxDetectionEAST.h"
//#include "fonttik/Configuration.hpp"
//#include "gmock/gmock.h"
//#include "fonttik/Log.h"
//
//namespace tik {
//	TEST(OverlapTests, Non_Overlapping) {
//		TextBox a(cv::Rect(0, 0, 2, 2));
//		TextBox b(cv::Rect(5, 5, 2, 2));
//		auto overlap = TextBox::OverlapAxisPercentage(a, b);
//		ASSERT_FLOAT_EQ(0.0, overlap.first);
//		ASSERT_FLOAT_EQ(0.0, overlap.second);
//	}
//
//	TEST(OverlapTests, Partial_Overlap) {
//		TextBox a(cv::Rect(0, 0, 4, 4));
//		TextBox b(cv::Rect(2, 3, 4, 4));
//		auto overlap = TextBox::OverlapAxisPercentage(a, b);
//		ASSERT_FLOAT_EQ(0.5, overlap.first);
//		ASSERT_FLOAT_EQ(0.25, overlap.second);
//	}
//
//	// A smaller rectangle contained inside a bigger one
//	//Should completely overlap
//	TEST(OverlapTests, Concentric_Overlap) {
//		TextBox a(cv::Rect(0, 0, 5, 5));
//		TextBox b(cv::Rect(2, 2, 2, 2));
//		auto overlap = TextBox::OverlapAxisPercentage(a, b);
//		ASSERT_FLOAT_EQ(1, overlap.first);
//		ASSERT_FLOAT_EQ(1, overlap.second);
//	}
//
//	//Two identical rectangles fully overlap
//	TEST(OverlapTests, Total_Overlap) {
//		TextBox a(cv::Rect(0, 0, 5, 5));
//		TextBox b(cv::Rect(0, 0, 5, 5));
//		auto overlap = TextBox::OverlapAxisPercentage(a, b);
//		ASSERT_FLOAT_EQ(1, overlap.first);
//		ASSERT_FLOAT_EQ(1, overlap.second);
//	}
//
//	TEST(OverlapTests, Commutative_Overlap) {
//		TextBox a(cv::Rect(0, 0, 5, 5));
//		TextBox b(cv::Rect(1, 1, 5, 5));
//		auto overlap1 = TextBox::OverlapAxisPercentage(a, b);
//		auto overlap2 = TextBox::OverlapAxisPercentage(b, a);
//		ASSERT_FLOAT_EQ(overlap1.first, overlap2.first);
//		ASSERT_FLOAT_EQ(overlap1.second, overlap2.second);
//	}
//
//	//Resulting textbox should cover the two created ones
//	TEST(MergeTests, Separate_Merge) {
//		TextBox a({ 0, 0, 2, 2 }),
//			b({ 2, 2, 2, 2 });
//		a.mergeWith(b);
//		ASSERT_EQ(a.getRect(), cv::Rect(0, 0, 4, 4));
//	}
//
//	//Textbox should remain unchanged if it is merged with a textbox that is already enclosed
//	TEST(MergeTests, Merge_Contained) {
//		TextBox a1({ 0, 0, 2, 2 }),
//			a2({ 0, 0, 2, 2 }),
//			b({ 0, 0, 1, 1 });
//		a1.mergeWith(b);
//		ASSERT_EQ(a2.getRect(), a1.getRect());
//	}
//
//	TEST(MergeTests, Commutative) {
//		TextBox a1({ 0, 0, 2, 2 }),
//			b1({ 4, 4, 2, 2 });
//		TextBox a2({ 0, 0, 2, 2 }),
//			b2({ 4, 4, 2, 2 });
//		a1.mergeWith(b1);
//		b2.mergeWith(a2);
//		ASSERT_EQ(a1.getRect(), b2.getRect());
//	}
//
//	class TextboxMergingTest : public ::testing::Test {
//	protected:
//		std::vector<TextBox> boxes;
//		TextDetectionParams params;
//		int initalBoxes;
//		void SetUp() override {
//			tik::Log::InitCoreLogger(false, false);
//			//100% overlap both axes
//			boxes.push_back(TextBox({ 0,0,2,2 }));
//			boxes.push_back(TextBox({ 0,0,4,4 }));
//			//20% overlap both axes
//			boxes.push_back(TextBox({ 5,5,10,10 }));
//			boxes.push_back(TextBox({ 13,13,10,10 }));
//			//70% overlap x 10% overlap y
//			boxes.push_back(TextBox({ 100,100,10,10 }));
//			boxes.push_back(TextBox({ 103,109,10,10 }));
//			//10% overlap x 70% overlap x
//			boxes.push_back(TextBox({ 200,200,10,10 }));
//			boxes.push_back(TextBox({ 209,203,10,10 }));
//
//			params = TextDetectionParams();
//			initalBoxes = boxes.size();
//		}
//
//
//	};
//
//	TEST_F(TextboxMergingTest, Merge_Any_Overlap)
//	{
//		params.mergeThreshold = std::make_pair(0.01, 0.01);
//		ITextboxDetection::mergeTextBoxes(boxes,&params);
//		ASSERT_THAT(boxes, ::testing::ElementsAre(TextBox({ 0, 0, 4, 4 }),
//			TextBox({ 5,5,18,18 }),
//			TextBox({100,100,13,19}),
//			TextBox({200, 200, 19, 13})));
//	}
//
//	TEST_F(TextboxMergingTest, Merge_50_Both)
//	{
//		params.mergeThreshold = std::make_pair(0.5, 0.5);
//		ITextboxDetection::mergeTextBoxes(boxes, &params);
//		ASSERT_THAT(boxes, ::testing::ElementsAre(
//			TextBox({ 0, 0, 4, 4 }),
//			TextBox({5, 5, 10, 10}),
//			TextBox({13, 13, 10, 10}),
//			TextBox({100, 100, 10, 10}),
//			TextBox({103, 109, 10, 10}),
//			TextBox({200, 200, 10, 10}),
//			TextBox({209, 203, 10, 10})));
//	}
//
//	TEST_F(TextboxMergingTest, Merge_x20_yAny)
//	{
//		params.mergeThreshold = std::make_pair(0.2, 0.0);
//		ITextboxDetection::mergeTextBoxes(boxes, &params);
//		ASSERT_THAT(boxes, ::testing::ElementsAre(
//			TextBox({ 0, 0, 4, 4 }),
//			TextBox({ 5,5,18,18 }),
//			TextBox({ 100,100,13,19 }),
//			TextBox({ 200, 200, 10, 10 }),
//			TextBox({ 209, 203, 10, 10 })));
//	}
//
//	TEST_F(TextboxMergingTest, Merge_xAny_y20)
//	{
//		params.mergeThreshold = std::make_pair(0.0, 0.2);
//		ITextboxDetection::mergeTextBoxes(boxes);
//		cv::Point2f{ 0, 0 };
//		TextBox t(cv::Rect{ cv::Point{ 0, 0 }, cv::Point{ 4, 4 } });
//
//		ASSERT_THAT(boxes, ::testing::ElementsAre(
//			,
//			TextBox({ 5,5,18,18 }),
//			TextBox({ 100, 100, 10, 10 }),
//			TextBox({ 103, 109, 10, 10 }),
//			TextBox({ 200, 200, 19, 13 })));
//	}
//}