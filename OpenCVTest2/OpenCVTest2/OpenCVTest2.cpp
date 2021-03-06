// OpenCVTest2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

// Declare structure to be used to pass data from C++ to Mono.
struct Circle
{
	Circle(int x, int y, int radius) : X(x), Y(y), Radius(radius) {}
	int X, Y, Radius;
};

CascadeClassifier _faceCascade;
String _windowName = "Unity OpenCV Interop Sample";
VideoCapture _capture;
int _scale = 1;

extern "C" int __declspec(dllexport) __stdcall  Init(int& outCameraWidth, int& outCameraHeight)
{
	// Load LBP face cascade.
	if (!_faceCascade.load("lbpcascade_frontalface.xml"))
		return -1;

	// Open the stream.
	_capture.open(0);
	if (!_capture.isOpened())
		return -2;

	outCameraWidth = (int)_capture.get(CAP_PROP_FRAME_WIDTH);
	outCameraHeight = (int)_capture.get(CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall  Close()
{
	_capture.release();
}

extern "C" void __declspec(dllexport) __stdcall SetScale(int scale)
{
	_scale = scale;
}

extern "C" int __declspec(dllexport) __stdcall Detect(Circle& outFace, uint8_t* outPixels, int outPixelsSize)
{
	Mat frame;
	_capture >> frame;
	if (frame.empty())
		return -1;

	std::vector<Rect> faces;
	// Convert the frame to grayscale for cascade detection.
	Mat grayscaleFrame;
	cvtColor(frame, grayscaleFrame, COLOR_BGR2GRAY);
	Mat resizedGray;
	// Scale down for better performance.
	resize(grayscaleFrame, resizedGray, Size(frame.cols / _scale, frame.rows / _scale));
	equalizeHist(resizedGray, resizedGray);

	// Detect faces.
	_faceCascade.detectMultiScale(resizedGray, faces);

	if (faces.size() == 0)
		return 0;

	int maxw = 0;
	size_t maxi = 0;
	for (size_t i = 0; i < faces.size(); i++) {
		faces[i].x *= _scale;
		faces[i].y *= _scale;
		faces[i].width *= _scale;
		faces[i].height *= _scale;
		if (faces[i].width > maxw) {
			maxw = faces[i].width;
			maxi = i;
		}
	}
	Rect face = faces[maxi];

	int cx = face.x + face.width / 2;
	int cy = face.y + face.height / 2;
	Rect rect(int(cx - face.width / 2 * 1.3), int(cy - face.height / 2 * 1.6), 
				int(face.width * 1.3), int(face.height * 1.7));

	Mat bgdModel, fgdModel, downsized, mask2;
	Mat mask(downsized.size(), CV_8UC1, GC_BGD);
	pyrDown(frame, downsized);
	Rect rect2(rect.x / 2, rect.y / 2, rect.width / 2, rect.height / 2);
	grabCut(downsized, mask, rect2, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);
	resize(mask, mask2, frame.size(), 0, 0, 0);

	vector<Mat> channels;
	Mat alpha = (mask2 & 1) * 255;
	split(frame, channels);
	channels.push_back(alpha);
	Mat rgba;
	merge(channels, rgba);

	Mat masked;
	frame.copyTo(masked, mask2 & 1);
	rectangle(masked, rect, Scalar(0, 255, 0));
	imshow(_windowName, masked);

	outFace = Circle(rect.x, rect.y, rect.width / 2);
	memcpy(outPixels, rgba.data, rgba.cols * rgba.rows * 4);

	//// Draw faces.
	//for (size_t i = 0; i < faces.size(); i++)
	//{
	//	Point center(_scale * (faces[i].x + faces[i].width / 2), _scale * (faces[i].y + faces[i].height / 2));
	//	ellipse(frame, center, Size(_scale * faces[i].width / 2, _scale * faces[i].height / 2), 0, 0, 360, Scalar(0, 0, 255), 4, 8, 0);

	//	// Send to application.
	//	outFaces[i] = Circle(faces[i].x, faces[i].y, faces[i].width / 2);
	//	outDetectedFacesCount++;

	//	if (outDetectedFacesCount == maxOutFacesCount)
	//		break;
	//}

	return 1;
}
