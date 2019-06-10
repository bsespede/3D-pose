#pragma once

#include "model/config/ConfigController.h"
#include "model/camera/CameraController.h"
#include "model/scene/SceneController.h"
#include "model/calibration/CalibrationController.h"

class AppController
{
public:
	AppController(ConfigController* fileController);
	bool hasScene(std::string name);
	Scene getScene(std::string name);
	Scene saveScene(std::string name);
	bool startCameras(CaptureType captureType);
	void stopCameras();
	void startCapturingVideo();
	void stopCapturingVideo();
	void updateSafeImage();
	Packet* getSafeImage();
	Video3D* getResult(Scene scene, CaptureType captureType);
	bool hasCapture(Scene scene, CaptureType captureType);
	void saveCapture(Scene scene, CaptureType captureType);
	bool calibrate(Scene scene, CalibrationType calibrationType);
private:
	ConfigController* configController;
	SceneController* sceneController;
	CameraController* cameraController;
	CalibrationController* calibrationController;
};
