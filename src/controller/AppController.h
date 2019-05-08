#pragma once

#include "controller/scene/SceneController.h"
#include "controller/camera/CameraController.h"
#include "controller/calibration/CalibrationController.h"
#include "model/util/Config.h"

using namespace std;

class AppController
{
public:
	AppController(Config* config);

	// Scene I/O
	bool sceneExists(string name);
	Scene createScene(string name);
	Scene loadScene(string name);
	bool hasCapture(Scene scene, Operation operation);
	void deleteCapture(Scene scene, Operation operation);

	// Cameras
	bool startCameras(CaptureMode captureMode);
	void stopCameras();
	void captureFrame();
	void startRecordingFrames();
	void stopRecordingFrames();
	void dumpCapture(Scene scene, Operation operation);
	void calculateIntrinsics(Scene scene);

	// Other
	FramesPacket* getSafeFrame();
	void updateSafeFrame();
	int getMaxCheckboards();
	void generateCheckboard();


private:
	SceneController* sceneController;
	CameraController* cameraController;
	CalibrationController* calibrationController;
};
