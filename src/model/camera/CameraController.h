#pragma once

#include <list>
#include <thread>
#include <atomic>
#include "model/enum/CaptureType.h"
#include "model/camera/optitrack/OptitrackCamera.h"
#include "model/camera/capture/Packet.h"
#include "model/camera/capture/Capture.h"
#include "model/config/ConfigController.h"

using namespace std;

class CameraController
{
public:
	CameraController(ConfigController* configController);
	bool startCameras(CaptureType captureType);
	void stopCameras();
	void startCapturingVideo();
	void stopCapturingVideo();
	Capture* getCapture();
	Packet* getSafeImage();
	void updateSafeImage();
private:
	void cameraLoop(int cameraFps);	
	int cameraLowFps;
	int cameraHighFps;
	Capture* capture;
	Packet* safeImage;
	OptitrackCamera* optitrackCamera;
	atomic<bool> shouldLoopThread;
	atomic<bool> shouldCaptureVideo;
	atomic<bool> shouldUpdateSafeImage;	
};
