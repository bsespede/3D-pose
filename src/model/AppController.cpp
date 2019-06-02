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

bool AppController::startCameras()
{
	return cameraController->startCameras();
}

void AppController::stopCameras()
{
	cameraController->stopCameras();
}

void AppController::startCapturingImage()
{
	cameraController->startCapturingImage();
}

void AppController::startCapturingVideo()
{
	cameraController->startCapturingVideo();
}

void AppController::stopCapturingVideo()
{
	cameraController->stopCapturingVideo();
}

bool AppController::hasCapture(Scene scene, Operation operation)
{
	return sceneController->hasCapture(scene, operation);
}

void AppController::saveCapture(Scene scene, Operation operation)
{
	sceneController->saveCapture(scene, operation, cameraController->getCapture());
}

bool AppController::calibrate(Scene scene, Operation operation)
{
	return calibrationController->calibrate(scene, operation);
}

Packet* AppController::getSafeImage()
{
	return cameraController->getSafeImage();
}

void AppController::updateSafeImage()
{
	cameraController->updateSafeImage();
}