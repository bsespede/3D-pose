#pragma once

#include <list>
#include <thread>
#include <atomic>
#include "model/capture/CaptureType.h"
#include "model/capture/Capture.h"
#include "model/camera/optitrack/OptitrackCamera.h"
#include "model/camera/Packet.h"
#include "model/config/ConfigController.h"

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
	void cameraLoop();
	Capture* capture;
	Packet* safeImage;
	OptitrackCamera* optitrackCamera;
	std::atomic<bool> shouldLoopThread;
	std::atomic<bool> shouldCaptureVideo;
	std::atomic<bool> shouldUpdateSafeImage;
};
