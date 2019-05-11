#pragma once

#include <map>
#include <thread>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <boost/log/trivial.hpp>
#include <cameralibrary.h>
#include "model/camera/FramesPacket.h"
#include "controller/files/FileController.h"

#define MAX_CAMERAS 16

using namespace std;
using namespace cv;
using namespace CameraLibrary;

class OptitrackCamera
{
public:
	OptitrackCamera(FileController* fileController);
	bool startCameras(Core::eVideoMode mode);
	FramesPacket* captureFramesPacket();
	void stopCameras();
	void shutdownCameras();
private:
	map<int, int> camerasOrder;
	int camerasFps;
	int cameraCount;
	atomic<int> frameCount;
	CameraList list;
	Camera* camera[MAX_CAMERAS];
	cModuleSync* sync;	
};
