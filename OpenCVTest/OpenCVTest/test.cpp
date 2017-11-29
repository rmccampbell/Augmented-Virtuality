#include "stdafx.h"
#include "BackgroundSubtraction.h"

namespace BackgroundSubtraction {
	cv::VideoCapture *cap;

	void Initialize() {
		cap = new cv::VideoCapture(0);
	}

	uint8_t* ProcessFrame() {
		cv::Mat image;
		cv::Mat betterImage;
		uint8_t* result;
		(*cap) >> image;
		cv::flip(image, image, 0);
		cv::cvtColor(image, betterImage, CV_BGR2BGRA, 4);
		result = new uint8_t[image.cols * image.rows * 4];
		memcpy(result, betterImage.data, betterImage.cols * image.rows * 4);
		return result;
	}
}
