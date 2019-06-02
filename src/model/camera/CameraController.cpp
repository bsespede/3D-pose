#include "CameraController.h"

CameraController::CameraController(ConfigController* configController)
{
	this->cameraFps = configController->getCameraFps();
	this->optitrackCamera = new OptitrackCamera(configController);
	this->capture = new Capture();
	this->safeImage = nullptr;
	this->shouldLoopThread = false;
	this->shouldCaptureVideo = false;
	this->shouldCaptureImage = false;
	this->shouldUpdateSafeImage = false;
}

bool CameraController::startCameras()
{
	if (optitrackCamera->startCameras())
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
	bool shouldKeepPacket = true;
	Packet* currentPacket = nullptr;

	while (shouldLoopThread)
	{
		if (!shouldKeepPacket)
		{
			delete currentPacket;
			shouldKeepPacket = true;
		}

		Packet* packet = optitrackCamera->getPacket();

		if (packet != nullptr)
		{
			currentPacket = packet;
			shouldKeepPacket = shouldCaptureVideo || shouldCaptureImage;

			if (shouldCaptureVideo)
			{
				capture->addPacket(currentPacket);
			}

			if (shouldCaptureImage)
			{
				capture->addPacket(currentPacket);
				shouldCaptureImage = false;
			}

			if (shouldUpdateSafeImage)
			{
				if (safeImage != nullptr)
				{
					delete safeImage;
				}			

				safeImage = new Packet(currentPacket);
				shouldUpdateSafeImage = false;
			}
		}

		int milisecondsToSleep = (int)(1.0 / cameraFps * 1000);
		this_thread::sleep_for(chrono::milliseconds(milisecondsToSleep));
	}
}

void CameraController::stopCameras()
{
	while (shouldCaptureImage || shouldCaptureVideo)
	{
		this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	shouldLoopThread = false;
	optitrackCamera->stopCameras();
}

void CameraController::startCapturingVideo()
{
	shouldCaptureVideo = true;
}

void CameraController::stopCapturingVideo()
{
	shouldCaptureVideo = false;
}

void CameraController::startCapturingImage()
{
	shouldCaptureImage = true;
}

void CameraController::updateSafeImage()
{
	shouldUpdateSafeImage = true;
}

Packet* CameraController::getSafeImage()
{
	if (shouldUpdateSafeImage)
	{
		return nullptr;
	}
	else
	{
		return safeImage;
	}
}

Capture* CameraController::getCapture()
{
	return capture;
}