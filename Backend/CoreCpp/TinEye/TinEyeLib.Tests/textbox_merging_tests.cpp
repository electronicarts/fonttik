#include <gtest/gtest.h>
#include "Textbox.h"
#include "TextboxDetectionEAST.h"
#include "Configuration.h"

namespace tin{
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
	TEST(OverlapTests,Concentric_Overlap) {
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
		auto overlap2 = Textbox::OverlapAxisPercentage(b,a);
		ASSERT_FLOAT_EQ(overlap1.first, overlap2.first);
		ASSERT_FLOAT_EQ(overlap1.second, overlap2.second);
	}

}