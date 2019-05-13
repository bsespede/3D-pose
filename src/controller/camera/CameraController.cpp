#include "CameraController.h"

CameraController::CameraController(FileController* fileController)
{
	this->camerasFps = fileController->getCamerasFps();
	this->optitrackCamera = new OptitrackCamera(fileController);
	this->capture = new Capture();
	this->safeFrame = nullptr;
	this->shouldUpdateSafeFrame = false;
	this->shouldLoopThread = false;
	this->shouldRecord = false;
	this->shouldSnap = false;	
}

bool CameraController::startCameras(CaptureMode mode)
{
	if (optitrackCamera->startCameras(mode.toOptitrackMode()))
	{
		if (capture != nullptr)
		{
			delete capture;
		}		
		capture = new Capture();

		shouldLoopThread = true;
		thread camerasThread = thread(&CameraController::cameraLoop, this);
		camerasThread.detach();
		return true;
	}
	
	return false;
}

void CameraController::cameraLoop()
{
	bool shouldKeepCurrFrame = true;
	FramesPacket* currentFrame = nullptr;

	while (shouldLoopThread)
	{
		if (!shouldKeepCurrFrame)
		{
			delete currentFrame;
			shouldKeepCurrFrame = true;
		}

		FramesPacket* capturedFrame = optitrackCamera->captureFramesPacket();

		if (capturedFrame != nullptr)
		{
			currentFrame = capturedFrame;
			shouldKeepCurrFrame = shouldRecord || shouldSnap;

			if (shouldRecord)
			{
				capture->addFrame(currentFrame);
			}

			if (shouldSnap)
			{
				capture->addFrame(currentFrame);
				shouldSnap = false;
			}

			if (shouldUpdateSafeFrame)
			{
				if (safeFrame != nullptr)
				{
					delete safeFrame;
				}			

				safeFrame = new FramesPacket(currentFrame);
				shouldUpdateSafeFrame = false;
			}
		}

		int milisecondsToSleep = (int)(1.0 / camerasFps * 1000);
		this_thread::sleep_for(chrono::milliseconds(milisecondsToSleep));
	}
}

void CameraController::stopCameras()
{
	while (shouldSnap || shouldRecord)
	{
		this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	shouldLoopThread = false;
	optitrackCamera->stopCameras();
}

void CameraController::startRecording()
{
	shouldRecord = true;
}

void CameraController::stopRecording()
{
	shouldRecord = false;
}

void CameraController::startSnap()
{
	shouldSnap = true;
}

void CameraController::updateSafeFrame()
{
	shouldUpdateSafeFrame = true;
}

FramesPacket* CameraController::getSafeFrame()
{
	if (shouldUpdateSafeFrame)
	{
		return nullptr;
	}
	else
	{
		return safeFrame;
	}
}

Capture* CameraController::getCapture()
{
	return capture;
}