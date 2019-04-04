#pragma once

#include <opencv2/opencv.hpp>
#include <boost/log/trivial.hpp>
#include "cameralibrary.h"
#include "model/camera/FramesPacket.h"

#define MAX_CAMERAS 32

using namespace std;
using namespace cv;
using namespace CameraLibrary;

class OptitrackCamera
{
public:
	OptitrackCamera();
	bool startCameras(Core::eVideoMode mode);
	FramesPacket captureFrames();
	void stopCameras();
private:
	CameraList list;
	Camera* camera[MAX_CAMERAS];
	cModuleSync* sync;
	int cameraCount;
};
