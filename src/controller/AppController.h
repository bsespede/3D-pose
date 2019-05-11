#pragma once

#include "controller/files/FileController.h"
#include "controller/camera/CameraController.h"
#include "controller/calibration/CalibrationController.h"

using namespace std;

class AppController
{
public:
	AppController(FileController* fileController);

	// Scene
	bool sceneExists(string name);
	Scene createScene(string name);
	Scene loadScene(string name);
	
	// Cameras
	bool startCameras(CaptureMode captureMode);
	void stopCameras();
	void startSnap();
	void startRecordingFrames();
	void stopRecordingFrames();
	void updateSafeFrame();
	FramesPacket* getSafeFrame();

	// Capture
	bool hasCapture(Scene scene, Operation operation);
	void saveCapture(Scene scene, Operation operation);

	// Calibration
	bool calibrate(Scene scene, Operation operation);
	int getMaxCheckboards();
private:
	FileController* fileController;
	CameraController* cameraController;
	CalibrationController* calibrationController;
};
