#pragma once

#include "controller/scene/SceneController.h"
#include "controller/camera/CameraController.h"
#include "controller/calibration/CalibrationController.h"

using namespace std;

class AppController
{
public:
	AppController(string dataPath, int maxCheckboards, int fps);

	// Scene I/O
	bool sceneExists(string name);
	Scene createScene(string name);
	Scene loadScene(string name);
	bool hasCapture(Scene scene, Operation operation);
	void deleteCapture(Scene scene, Operation operation);

	// Cameras
	bool startCapturing(CaptureMode captureMode);
	void stopCapturing();
	void captureFrame();
	void startRecordingFrames();
	void stopRecordingFrames();
	void dumpCapture(Scene scene, Operation operation);

	// Other
	FramesPacket getCurrentFrame();
	int getCamerasFps();
	int getMaxCheckboards();
private:
	SceneController* sceneController;
	CameraController* cameraController;
	CalibrationController* calibrationController;
};
