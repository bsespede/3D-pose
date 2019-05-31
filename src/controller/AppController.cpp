#include "AppController.h"

AppController::AppController(FileController* fileController)
{
	this->fileController = fileController;
	this->calibrationController = new CalibrationController(fileController);
	this->cameraController = new CameraController(fileController);
	calibrationController->generateCheckboard();
}

bool AppController::sceneExists(string name)
{
	return fileController->sceneExists(name);
}

Scene AppController::createScene(string name)
{
	return fileController->createScene(name);
}

Scene AppController::loadScene(string name)
{
	return fileController->loadScene(name);
}

bool AppController::startCameras(CaptureMode captureMode)
{
	return cameraController->startCameras(captureMode);
}

void AppController::stopCameras()
{
	cameraController->stopCameras();
}

void AppController::startSnap()
{
	cameraController->startSnap();
}

void AppController::startRecordingFrames()
{
	cameraController->startRecording();
}

void AppController::stopRecordingFrames()
{
	cameraController->stopRecording();
}

bool AppController::hasCapture(Scene scene, Operation operation)
{
	return fileController->hasCapture(scene, operation);
}

void AppController::saveCapture(Scene scene, Operation operation)
{
	fileController->saveCapture(scene, operation, cameraController->getCapture());
}

bool AppController::calibrate(Scene scene, Operation operation)
{
	return calibrationController->calibrate(scene, operation);
}

FramesPacket* AppController::getSafeFrame()
{
	return cameraController->getSafeFrame();
}

void AppController::updateSafeFrame()
{
	cameraController->updateSafeFrame();
}

int AppController::getMaxCheckboards()
{
	return fileController->getMaxCheckboards();
}