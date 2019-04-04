#pragma once

#include <list>
#include <thread>
#include <atomic>

#include "controller/camera/optitrack/OptitrackCamera.h"
#include "model/camera/FramesPacket.h"
#include "model/camera/enum/CaptureMode.h"

using namespace std;

class CameraController
{
	OptitrackCamera optitrackCamera;
	list<FramesPacket> recording;
	FramesPacket currentFrames;
	atomic<bool> isCapturing;
	atomic<bool> isRecording;		
	void capture();
	int camerasFps;	
public:
	// Hardware control
	CameraController(int camerasFps);
	bool startCapturing(CaptureMode mode);
	void stopCapturing();

	// Video control
	void startRecording();
	void stopRecording();
	list<FramesPacket> getRecording();

	// Frame control
	FramesPacket getCurrentFrames();

	// Other
	int getCamerasFps();
};
