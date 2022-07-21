//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "Textbox.h"
#include "TextboxDetectionEAST.h"
#include "Configuration.h"
#include "gmock/gmock.h"
#include "Log.h"

namespace tin {
	TEST(OverlapTests, Non_Overlapping) {
		Textbox a(cv::Rect(0, 0, 2, 2));
		Textbox b(cv::Rect(5, 5, 2, 2));
		auto overlap = Textbox::OverlapAxisPercentage(a, b);
		ASSERT_FLOAT_EQ(0.0, overlap.first);
		ASSERT_FLOAT_EQ(0.0, overlap.second);
	}

	TEST(OverlapTests, Partial_Overlap) {
		Textbox a(cv::Rect(0, 0, 4, 4));
		Textbox b(cv::Rect(2, 3, 4, 4));
		auto overlap = Textbox::OverlapAxisPercentage(a, b);
		ASSERT_FLOAT_EQ(0.5, overlap.first);
		ASSERT_FLOAT_EQ(0.25, overlap.second);
	}

	// A smaller rectangle contained inside a bigger one
	//Should completely overlap
	TEST(OverlapTests, Concentric_Overlap) {
		Textbox a(cv::Rect(0, 0, 5, 5));
		Textbox b(cv::Rect(2, 2, 2, 2));
		auto overlap = Textbox::OverlapAxisPercentage(a, b);
		ASSERT_FLOAT_EQ(1, overlap.first);
		ASSERT_FLOAT_EQ(1, overlap.second);
	}

	//Two identical rectangles fully overlap
	TEST(OverlapTests, Total_Overlap) {
		Textbox a(cv::Rect(0, 0, 5, 5));
		Textbox b(cv::Rect(0, 0, 5, 5));
		auto overlap = Textbox::OverlapAxisPercentage(a, b);
		ASSERT_FLOAT_EQ(1, overlap.first);
		ASSERT_FLOAT_EQ(1, overlap.second);
	}

	TEST(OverlapTests, Commutative_Overlap) {
		Textbox a(cv::Rect(0, 0, 5, 5));
		Textbox b(cv::Rect(1, 1, 5, 5));
		auto overlap1 = Textbox::OverlapAxisPercentage(a, b);
		auto overlap2 = Textbox::OverlapAxisPercentage(b, a);
		ASSERT_FLOAT_EQ(overlap1.first, overlap2.first);
		ASSERT_FLOAT_EQ(overlap1.second, overlap2.second);
	}

	//Resulting textbox should cover the two created ones
	TEST(MergeTests, Separate_Merge) {
		Textbox a({ 0, 0, 2, 2 }),
			b({ 2, 2, 2, 2 });
		a.mergeWith(b);
		ASSERT_EQ(a.getRect(), cv::Rect(0, 0, 4, 4));
	}

	//Textbox should remain unchanged if it is merged with a textbox that is already enclosed
	TEST(MergeTests, Merge_Contained) {
		Textbox a1({ 0, 0, 2, 2 }),
			a2({ 0, 0, 2, 2 }),
			b({ 0, 0, 1, 1 });
		a1.mergeWith(b);
		ASSERT_EQ(a2.getRect(), a1.getRect());
	}

	TEST(MergeTests, Commutative) {
		Textbox a1({ 0, 0, 2, 2 }),
			b1({ 4, 4, 2, 2 });
		Textbox a2({ 0, 0, 2, 2 }),
			b2({ 4, 4, 2, 2 });
		a1.mergeWith(b1);
		b2.mergeWith(a2);
		ASSERT_EQ(a1.getRect(), b2.getRect());
	}

	class TextboxMergingTest : public ::testing::Test {
	protected:
		std::vector<Textbox> boxes;
		TextDetectionParams params;
		int initalBoxes;
		void SetUp() override {
			tin::Log::InitCoreLogger(false, false);
			//100% overlap both axes
			boxes.push_back(Textbox({ 0,0,2,2 }));
			boxes.push_back(Textbox({ 0,0,4,4 }));
			//20% overlap both axes
			boxes.push_back(Textbox({ 5,5,10,10 }));
			boxes.push_back(Textbox({ 13,13,10,10 }));
			//70% overlap x 10% overlap y
			boxes.push_back(Textbox({ 100,100,10,10 }));
			boxes.push_back(Textbox({ 103,109,10,10 }));
			//10% overlap x 70% overlap x
			boxes.push_back(Textbox({ 200,200,10,10 }));
			boxes.push_back(Textbox({ 209,203,10,10 }));

			params = TextDetectionParams();
			initalBoxes = boxes.size();
		}


	};

	TEST_F(TextboxMergingTest, Merge_Any_Overlap)
	{
		params.setMergeThreshold(std::make_pair(0.01, 0.01));
		ITextboxDetection::mergeTextBoxes(boxes,&params);
		ASSERT_THAT(boxes, ::testing::ElementsAre(Textbox({ 0, 0, 4, 4 }),
			Textbox({ 5,5,18,18 }),
			Textbox({100,100,13,19}),
			Textbox({200, 200, 19, 13})));
	}

	TEST_F(TextboxMergingTest, Merge_50_Both)
	{
		params.setMergeThreshold(std::make_pair(0.5, 0.5));
		ITextboxDetection::mergeTextBoxes(boxes, &params);
		ASSERT_THAT(boxes, ::testing::ElementsAre(
			Textbox({ 0, 0, 4, 4 }),
			Textbox({5, 5, 10, 10}),
			Textbox({13, 13, 10, 10}),
			Textbox({100, 100, 10, 10}),
			Textbox({103, 109, 10, 10}),
			Textbox({200, 200, 10, 10}),
			Textbox({209, 203, 10, 10})));
	}

	TEST_F(TextboxMergingTest, Merge_x20_yAny)
	{
		params.setMergeThreshold(std::make_pair(0.2, 0.0));
		ITextboxDetection::mergeTextBoxes(boxes, &params);
		ASSERT_THAT(boxes, ::testing::ElementsAre(
			Textbox({ 0, 0, 4, 4 }),
			Textbox({ 5,5,18,18 }),
			Textbox({ 100,100,13,19 }),
			Textbox({ 200, 200, 10, 10 }),
			Textbox({ 209, 203, 10, 10 })));
	}

	TEST_F(TextboxMergingTest, Merge_xAny_y20)
	{
		params.setMergeThreshold(std::make_pair(0.0, 0.2));
		ITextboxDetection::mergeTextBoxes(boxes, &params);
		ASSERT_THAT(boxes, ::testing::ElementsAre(
			Textbox({ 0, 0, 4, 4 }),
			Textbox({ 5,5,18,18 }),
			Textbox({ 100, 100, 10, 10 }),
			Textbox({ 103, 109, 10, 10 }),
			Textbox({ 200, 200, 19, 13 })));
	}
}