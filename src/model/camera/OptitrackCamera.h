#pragma once

#include <atomic>
#include <opencv2/opencv.hpp>
#include <boost/log/trivial.hpp>
#include "model/scene/enum/CaptureMode.h"
#include "cameralibrary.h"

#define MAX_CAMERAS 16
#define CAMERA_FPS 30

class OptitrackCamera
{
private:
	CameraLibrary::CameraList list;
	CameraLibrary::Camera* camera[MAX_CAMERAS];
	CameraLibrary::cModuleSync* sync;
	int cameraCount;
public:
	OptitrackCamera();
	bool initialize();
	void start(CaptureMode mode);
	std::map<int, cv::Mat> capture();
	void stop();
	void release();
};
