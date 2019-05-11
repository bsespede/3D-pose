#pragma once

#include <list>
#include <thread>
#include <atomic>
#include "model/camera/FramesPacket.h"
#include "model/camera/enum/CaptureMode.h"
#include "model/capture/Capture.h"
#include "controller/files/FileController.h"
#include "controller/camera/optitrack/OptitrackCamera.h"

using namespace std;

class CameraController
{
public:
	// Hardware control
	CameraController(FileController* fileController);
	bool startCameras(CaptureMode mode);
	void stopCameras();

	// Capture control
	void startRecording();
	void stopRecording();
	void startSnap();
	Capture* getCapture();

	// Frame control
	FramesPacket* getSafeFrame();
	void updateSafeFrame();
private:
	void cameraLoop();
	FileController* fileController;
	OptitrackCamera* optitrackCamera;
	Capture* capture;
	FramesPacket* safeFrame;
	atomic<bool> shouldUpdateSafeFrame;
	atomic<bool> shouldLoopThread;
	atomic<bool> shouldRecord;
	atomic<bool> shouldSnap;
	int camerasFps;
};
