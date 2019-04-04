#pragma once

#include "scene/SceneController.h"
#include "camera/CameraController.h"

using namespace std;

class AppController
{
private:
	string dataPath;
public:
	AppController(string dataPath);

	// Scene I/O
	bool sceneExists(string name);
	Scene createScene(string name);
	Scene loadScene(string name);
	bool hasCapture(Scene scene, Operation operation);
	void deleteCapture(Scene scene, Operation operation);

	// Cameras
	bool startCameras(Scene scene, Operation operation);
	void captureFrame(Scene scene, Operation operation);
	void startRecordingFrames(Scene scene, Operation operation);
	void stopRecordingFrames(Scene scene, Operation operation);
	void dumpCapture(Scene scene, Operation operation);
	int getCameraFps();
	Mat getCurrentFrame();

	// Other
	int getMaxCheckboards();
};
