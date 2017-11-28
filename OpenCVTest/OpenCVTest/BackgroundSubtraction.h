#pragma once

#include "stdafx.h"
using namespace std;
using namespace cv;

namespace BackgroundSubtraction {
	extern "C" { __declspec(dllexport) void Initialize(); }
	extern "C" { __declspec(dllexport) uint8_t* ProcessFrame(); }
}
