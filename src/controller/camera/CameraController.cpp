#include "CameraController.h"

CameraController::CameraController(int camerasFps): optitrackCamera(OptitrackCamera()), capture(Capture()), isCapturing(false), isRecording(true), captureNextFrame(false), camerasFps(camerasFps)
{

}

bool CameraController::startCapturing(CaptureMode mode)
{
	if (optitrackCamera.startCameras(mode.toOptitrackMode()))
	{
		capture = Capture();
		isCapturing = true;
		thread captureThread = thread(&CameraController::captureThread, this);
		captureThread.detach();
		return true;
	}
	else
	{
		return false;
	}	
}

void CameraController::captureThread()
{
	while (isCapturing)
	{
		int milisecondsToSleep = (int)(1.0 / camerasFps) * 1000;
		chrono::system_clock::time_point timePoint = chrono::system_clock::now() + chrono::milliseconds(milisecondsToSleep);

		FramesPacket frame = optitrackCamera.captureFrames();

		if (isRecording)
		{
			capture.addToCaptureRecording(frame);
		}

		if (captureNextFrame)
		{
			capture.addToCaptureFrame(frame);
			captureNextFrame = false;
		}

		currentFrame = frame;

		this_thread::sleep_until(timePoint);
	}
}

void CameraController::stopCapturing()
{
	isCapturing = false;
	optitrackCamera.stopCameras();
}

void CameraController::startRecording()
{
	isRecording = true;
}

void CameraController::stopRecording()
{
	isRecording = false;
}

void CameraController::captureFrame()
{
	captureNextFrame = true;
}

FramesPacket CameraController::getCurrentFrame()
{
	return currentFrame;
}

int CameraController::getCamerasFps()
{
	return camerasFps;
}

Capture CameraController::getCapture()
{
	return capture;
}