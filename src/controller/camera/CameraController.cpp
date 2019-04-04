#include "CameraController.h"

CameraController::CameraController(int camerasFps): optitrackCamera(OptitrackCamera()), recording(list<FramesPacket>()), isCapturing(false), isRecording(true), camerasFps(camerasFps)
{

}

bool CameraController::startCapturing(CaptureMode mode)
{
	if (optitrackCamera.startCameras(mode.toOptitrackMode()))
	{
		isCapturing = true;
		thread captureThread = thread(&CameraController::capture, this);
		captureThread.detach();
		return true;
	}
	else
	{
		return false;
	}	
}

void CameraController::stopCapturing()
{
	isCapturing = false;
	optitrackCamera.stopCameras();
}

void CameraController::startRecording()
{
	recording.clear();
	isRecording = true;
}

void CameraController::capture()
{
	while (isCapturing)
	{
		int milisecondsToSleep = (int)(1.0 / camerasFps) * 1000;
		chrono::system_clock::time_point timePoint = chrono::system_clock::now() + chrono::milliseconds(milisecondsToSleep);

		FramesPacket frames = optitrackCamera.captureFrames();
		if (isRecording)
		{
			recording.push_back(frames);
		}
		currentFrames = frames;

		this_thread::sleep_until(timePoint);
	}
}

void CameraController::stopRecording()
{
	isRecording = false;
}

list<FramesPacket> CameraController::getRecording()
{
	return recording;
}

FramesPacket CameraController::getCurrentFrames()
{
	return currentFrames;
}

int CameraController::getCamerasFps()
{
	return camerasFps;
}