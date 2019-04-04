#pragma once

#include <list>
#include <thread>
#include <atomic>

#include "controller/camera/optitrack/OptitrackCamera.h"
#include "model/camera/FramesPacket.h"
#include "model/camera/enum/CaptureMode.h"
#include "model/capture/Capture.h"

using namespace std;

class CameraController
{
	OptitrackCamera optitrackCamera;
	FramesPacket currentFrame;
	atomic<bool> isCapturing;
	atomic<bool> isRecording;
	atomic<bool> captureNextFrame;
	Capture capture;
	void captureThread();
	int camerasFps;	
public:
	// Hardware control
	CameraController(int camerasFps);
	bool startCapturing(CaptureMode mode);
	void stopCapturing();

	// Capture control
	void startRecording();
	void stopRecording();
	void captureFrame();
	Capture getCapture();

	// Frame control
	FramesPacket getCurrentFrame();

	// Other
	int getCamerasFps();
};
