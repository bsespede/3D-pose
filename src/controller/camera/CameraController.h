#pragma once

#include <list>
#include <thread>
#include <atomic>

#include "controller/camera/enum/CaptureMode.h"
#include "controller/camera/optitrack/OptitrackCamera.h"
#include "model/camera/FramesPacket.h"

using namespace std;

class CameraController
{
	OptitrackCamera optitrackCamera;
	list<FramesPacket> recording;
	atomic<bool> shouldRecord;
	atomic<bool> stoppedRecording;		
	void record();
	int camerasFps;	
public:
	// Hardware control
	CamerasController(int camerasFps);
	bool startCameras(CaptureMode mode);
	void stopCameras();

	// Video control
	void startRecording();
	void stopRecording();
	bool finishedRecording();
	list<FramesPacket> getRecording();

	// Frame control
	FramesPacket getFrames();
};
