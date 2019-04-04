#include "AppController.h"

AppController::AppController(string dataPath, int camerasFps, int maxCheckboards): sceneController(SceneController(dataPath)), cameraController(CameraController(camerasFps)), calibrationController(CalibrationController(maxCheckboards))
{
	
}

bool AppController::sceneExists(string name)
{
	return sceneController.sceneExists(name);
}

Scene AppController::createScene(string name)
{
	return sceneController.createScene(name);
}

Scene AppController::loadScene(string name)
{
	return sceneController.loadScene(name);
}

bool AppController::hasCapture(Scene scene, Operation operation)
{
	return sceneController.hasCapture(scene, operation);
}

void AppController::deleteCapture(Scene scene, Operation operation)
{
	sceneController.deleteCapture(scene, operation);
}

bool AppController::startCapturing(CaptureMode captureMode)
{
	return cameraController.startCapturing(captureMode);
}

void AppController::stopCapturing()
{
	cameraController.stopCapturing();
}

void AppController::captureFrame(Scene scene, Operation operation);
void AppController::startRecordingFrames(Scene scene, Operation operation);
void AppController::stopRecordingFrames(Scene scene, Operation operation);
void AppController::dumpCapture(Scene scene, Operation operation);

int AppController::getCamerasFps()
{
	return cameraController.getCamerasFps();
}

FramesPacket AppController::getCurrentFrames()
{
	return cameraController.getCurrentFrames();
}

int AppController::getMaxCheckboards()
{
	return calibrationController.getMaxCheckboards();
}