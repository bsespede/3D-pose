#pragma once

#include "model/config/ConfigController.h"
#include "model/camera/CameraController.h"
#include "model/scene/SceneController.h"
#include "model/calibration/CalibrationController.h"

using namespace std;

class AppController
{
public:
	AppController(ConfigController* fileController);
	bool hasScene(string name);
	Scene getScene(string name);
	Scene saveScene(string name);
	bool startCameras();
	void stopCameras();
	void startCapturingImage();
	void startCapturingVideo();
	void stopCapturingVideo();
	void updateSafeImage();
	Packet* getSafeImage();
	bool hasCapture(Scene scene, Operation operation);
	void saveCapture(Scene scene, Operation operation);
	bool calibrate(Scene scene, Operation operation);
private:
	ConfigController* configController;
	SceneController* sceneController;
	CameraController* cameraController;
	CalibrationController* calibrationController;
};
