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
	atomic<bool> shouldRecord;
	atomic<bool> stoppedRecording;		
	void record();
	int camerasFps;	
public:
	// Hardware control
	CameraController(int camerasFps);
	bool startCameras(CaptureMode mode);
	void stopCameras();

	// Video control
	void startRecording();
	void stopRecording();
	bool finishedRecording();
	list<FramesPacket> getRecording();

	// Frame control
	FramesPacket getCurrentFrames();
};
