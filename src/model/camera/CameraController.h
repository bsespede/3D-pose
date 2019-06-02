#pragma once

#include <list>
#include <thread>
#include <atomic>

#include "model/camera/optitrack/OptitrackCamera.h"
#include "model/camera/capture/Packet.h"
#include "model/camera/capture/Capture.h"
#include "model/config/ConfigController.h"

using namespace std;

class CameraController
{
public:
	CameraController(ConfigController* configController);
	bool startCameras();
	void stopCameras();
	void startCapturingVideo();
	void stopCapturingVideo();
	void startCapturingImage();
	Capture* getCapture();
	Packet* getSafeImage();
	void updateSafeImage();
private:
	void cameraLoop();
	OptitrackCamera* optitrackCamera;
	Packet* safeImage;
	Capture* capture;
	atomic<bool> shouldLoopThread;
	atomic<bool> shouldCaptureVideo;
	atomic<bool> shouldCaptureImage;
	atomic<bool> shouldUpdateSafeImage;
	int cameraFps;
};
