#pragma once

#include "scene/SceneController.h"
#include "camera/CameraController.h"
#include "calibration/calibrationController.h"

using namespace std;

class AppController
{
private:
	SceneController sceneController;
	CameraController cameraController;
	CalibrationController calibrationController;
public:
	AppController(string dataPath, int camerasFps, int maxCheckboards);

	// Scene I/O
	bool sceneExists(string name);
	Scene createScene(string name);
	Scene loadScene(string name);
	bool hasCapture(Scene scene, Operation operation);
	void deleteCapture(Scene scene, Operation operation);

	// Cameras
	bool startCapturing(CaptureMode captureMode);
	void stopCapturing();
	void captureFrame(Scene scene, Operation operation);
	void startRecordingFrames(Scene scene, Operation operation);
	void stopRecordingFrames(Scene scene, Operation operation);
	void dumpCapture(Scene scene, Operation operation);

	// Other
	FramesPacket getCurrentFrames();
	int getCamerasFps();
	int getMaxCheckboards();
};
