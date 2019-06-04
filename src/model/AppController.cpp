#include "AppController.h"

AppController::AppController(ConfigController* configController)
{
	this->sceneController = new SceneController(configController->getDataFolder());
	this->calibrationController = new CalibrationController(configController, sceneController);
	this->cameraController = new CameraController(configController);
}

bool AppController::hasScene(string name)
{
	return sceneController->hasScene(name);
}

Scene AppController::saveScene(string name)
{
	return sceneController->saveScene(name);
}

Scene AppController::getScene(string name)
{
	return sceneController->getScene(name);
}

bool AppController::startCameras(CaptureType captureType)
{
	return cameraController->startCameras(captureType);
}

void AppController::stopCameras()
{
	cameraController->stopCameras();
}

void AppController::startCapturingVideo()
{
	cameraController->startCapturingVideo();
}

void AppController::stopCapturingVideo()
{
	cameraController->stopCapturingVideo();
}

Video3D* AppController::getResult(Scene scene, CaptureType captureType)
{
	return sceneController->getResult(scene, captureType);
}

bool AppController::hasCapture(Scene scene, CaptureType captureType)
{
	return sceneController->hasCapture(scene, captureType);
}

void AppController::saveCapture(Scene scene, CaptureType captureType)
{
	sceneController->saveCapture(scene, captureType, cameraController->getCapture());
}

bool AppController::calibrate(Scene scene, CalibrationType calibrationType)
{
	return calibrationController->calibrate(scene, calibrationType);
}

Packet* AppController::getSafeImage()
{
	return cameraController->getSafeImage();
}

void AppController::updateSafeImage()
{
	cameraController->updateSafeImage();
}