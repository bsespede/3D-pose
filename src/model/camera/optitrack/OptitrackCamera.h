#pragma once

#include <map>
#include <thread>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <boost/log/trivial.hpp>
#include <cameralibrary.h>
#include "model/camera/capture/Packet.h"
#include "model/config/ConfigController.h"

#define MAX_CAMERAS 16

using namespace std;
using namespace cv;
using namespace CameraLibrary;

class OptitrackCamera
{
public:
	OptitrackCamera(ConfigController* configController);
	bool startCameras();
	void stopCameras();
	void shutdownCameras();
	Packet* getPacket();
private:
	int cameraFps;
	int cameraWidth;
	int cameraHeight;
	map<int, int> cameraOrder;
	int cameraCount;
	CameraList list;
	Camera* camera[MAX_CAMERAS];
	cModuleSync* sync;	
};
