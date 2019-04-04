#include "CameraController.h"

CameraController::CameraController(int camerasFps) : optitrackCamera(OptitrackCamera()), currentFrame(FramesPacket()), capture(Capture()), camerasFps(camerasFps)
{
}

bool CameraController::startCapturing(CaptureMode mode)
{
	if (optitrackCamera.startCameras(mode.toOptitrackMode()))
	{
		capture = Capture();
		isCapturing = true;
		std::thread captureThread = std::thread(&CameraController::captureThread, this);
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
		std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now() + std::chrono::milliseconds(milisecondsToSleep);

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

		std::this_thread::sleep_until(timePoint);
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