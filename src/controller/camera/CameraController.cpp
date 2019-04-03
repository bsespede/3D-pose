#include "CameraController.h"

CameraController::CamerasController(int camerasFps)
{
	this->optitrackCamera = OptitrackCamera();
	this->recording = list<FramesPacket>();
	this->shouldRecord = false;
	this->stoppedRecording = true;
	this->camerasFps = camerasFps;	
}

bool CameraController::startCameras(CaptureMode mode)
{
	return optitrackCamera.startCameras(mode.toOptitrackMode());
}

void CameraController::stopCameras()
{
	optitrackCamera.stopCameras();
}

void CameraController::startRecording()
{
	recording.clear();

	shouldRecord = true;
	stoppedRecording = false;

	thread captureThread = thread(&CameraController::record, this);
	captureThread.detach();
}

void CameraController::record()
{
	while (shouldRecord)
	{
		int milisecondsToSleep = (int)(1.0 / cameraFps);
		chrono::system_clock::time_point timePoint = chrono::system_clock::now() + chrono::milliseconds(milisecondsToSleep);

		FramesPacket frames = optitrackCamera.captureFrames();
		recording.push_back(frames);

		this_thread::sleep_until(timePoint);
	}

	stoppedRecording = true;
}

void CameraController::stopRecording()
{
	shouldRecord = false;
}

bool CameraController::finishedRecording()
{
	return stoppedRecording;
}

list<FramesPacket> CameraController::getRecording()
{
	return recording;
}

FramesPacket CameraController::getFrames()
{
	return optitrackCamera.capture();
}