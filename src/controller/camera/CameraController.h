#pragma once

#include <list>
#include <thread>
#include <atomic>
#include <mutex>

#include "controller/camera/optitrack/OptitrackCamera.h"
#include "model/camera/FramesPacket.h"
#include "model/camera/enum/CaptureMode.h"
#include "model/capture/Capture.h"

using namespace std;

class CameraController
{
public:
	// Hardware control
	CameraController(int camerasFps);
	bool startCameras(CaptureMode mode);
	void stopCameras();

	// Capture control
	void startRecording();
	void stopRecording();
	void captureFrame();
	Capture* getCapture();

	// Frame control
	FramesPacket getSafeFrame();
	void updateSafeFrame();

	// Other
	int getCamerasFps();
private:
	OptitrackCamera* optitrackCamera;
	Capture* capture;
	FramesPacket safeFrame;
	atomic<bool> shouldUpdateSafeFrame;
	atomic<bool> shouldLoopThread;
	atomic<bool> shouldRecord;
	atomic<bool> shouldCapture;
	mutex racePreventer;
	int camerasFps;
	void cameraLoop();
};
