#pragma once

#include <map>
#include <thread>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <boost/log/trivial.hpp>
#include <cameralibrary.h>
#include "model/camera/Packet.h"
#include "model/config/ConfigController.h"

#define MAX_CAMERAS 16

class OptitrackCamera
{
public:
	OptitrackCamera(ConfigController* configController);
	bool startCameras(int cameraFps);
	void stopCameras();
	void shutdownCameras();
	Packet* getPacket();
private:
	std::map<int, int> cameraOrder;
	std::map<int, int> camerasWidth;
	std::map<int, int> camerasHeight;
	int cameraCount;
	CameraLibrary::CameraList list;
	CameraLibrary::Camera* camera[MAX_CAMERAS];
	CameraLibrary::cModuleSync* sync;
};
