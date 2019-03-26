#pragma once

#include <opencv2/opencv.hpp>
#include <boost/log/trivial.hpp>
#include "cameralibrary.h"

#define MAX_CAMERAS 16

class OptitrackCamera
{
private:
	CameraLibrary::CameraList list;
	CameraLibrary::Camera* camera[MAX_CAMERAS];
	CameraLibrary::cModuleSync* sync;
	int cameraCount;
public:
	OptitrackCamera();
	int initialize();
	int start(CaptureMode mode);
	std::map<int, cv::Mat> capture();
	int stop();
	int release();
};
