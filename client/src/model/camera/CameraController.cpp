#include "CameraController.h"

CameraController::CameraController(ConfigController* configController)
{
	this->cameraHighFps = configController->getCameraHighFps();
	this->cameraLowFps = configController->getCameraLowFps();
	this->optitrackCamera = new OptitrackCamera(configController);
	this->capture = new Capture();
	this->safeImage = nullptr;
	this->shouldLoopThread = false;
	this->shouldCaptureVideo = false;
	this->shouldUpdateSafeImage = false;
}

bool CameraController::startCameras(CaptureType captureType)
{
	int cameraFps = (captureType == CaptureType::CALIBRATION)? cameraLowFps : cameraHighFps;

	if (optitrackCamera->startCameras(cameraFps))
	{
		if (capture != nullptr)
		{
			delete capture;
		}		

		capture = new Capture();

		shouldLoopThread = true;
		std::thread camerasThread = std::thread(&CameraController::cameraLoop, this, cameraFps);
		camerasThread.detach();
		return true;
	}
	
	return false;
}

void CameraController::cameraLoop(int cameraFps)
{
	bool shouldKeepPacket = true;
	Packet* currentPacket = nullptr;

	while (shouldLoopThread)
	{
		int milisecondsToSleep = (int)(1.0 / cameraFps * 1000);
		std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now() + std::chrono::milliseconds(milisecondsToSleep);

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

		std::this_thread::sleep_until(timePoint);
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