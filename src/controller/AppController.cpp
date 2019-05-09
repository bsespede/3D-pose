#include "AppController.h"

AppController::AppController(Config* config)
{
	this->sceneController = new SceneController(config->getDataPath());
	this->calibrationController = new CalibrationController(config);
	this->cameraController = new CameraController(config);
}

bool AppController::sceneExists(string name)
{
	return sceneController->sceneExists(name);
}

Scene AppController::createScene(string name)
{
	return sceneController->createScene(name);
}

Scene AppController::loadScene(string name)
{
	return sceneController->loadScene(name);
}

bool AppController::startCameras(CaptureMode captureMode)
{
	return cameraController->startCameras(captureMode);
}

void AppController::stopCameras()
{
	cameraController->stopCameras();
}

void AppController::captureFrame()
{
	cameraController->captureFrame();
}

void AppController::startRecordingFrames()
{
	cameraController->startRecording();
}

void AppController::stopRecordingFrames()
{
	cameraController->stopRecording();
}

void AppController::dumpCapture(Scene scene, Operation operation)
{
	sceneController->saveCapture(scene, operation, cameraController->getCapture());
}

void AppController::calculateIntrinsics(Scene scene)
{
	vector<string> camerasPath = sceneController->getCaptureFolders(scene, Operation::INTRINSICS);
	vector<IntrinsicCalibration> intrinsicMatrices = calibrationController->calculateIntrinsics(camerasPath);

	sceneController->dumpIntrinsics(intrinsicMatrices);
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
	return calibrationController->getMaxCheckboards();
}