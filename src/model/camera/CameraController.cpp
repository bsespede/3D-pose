#include "CameraController.h"

CameraController::CameraController(ConfigController* configController)
{
	this->optitrackCamera = new OptitrackCamera(configController);
	this->capture = new Capture();
	this->safeImage = nullptr;
	this->shouldLoopThread = false;
	this->shouldCaptureVideo = false;
	this->shouldUpdateSafeImage = false;
}

bool CameraController::startCameras(CaptureType captureType)
{
	if (optitrackCamera->startCameras())
	{
		if (capture != nullptr)
		{
			delete capture;
		}		

		capture = new Capture();

		shouldLoopThread = true;
		std::thread camerasThread = std::thread(&CameraController::cameraLoop, this);
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
			shouldKeepPacket = shouldCaptureVideo;

			if (shouldCaptureVideo)
			{
				capture->addPacket(currentPacket);
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
	}
}

void CameraController::stopCameras()
{
	while (shouldCaptureVideo)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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