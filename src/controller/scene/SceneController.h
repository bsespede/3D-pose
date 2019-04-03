#pragma once

#include <string>

using namespace std;

class SceneController
{
private:
	string dataPath;
public:
	SceneController(string dataPath);

	// I/O scenes
	bool sceneExists(string name);
	Scene loadScene(string name);
	Scene createScene(string name);

	// Check existence
	bool hasSceneCapture(Scene scene);
	bool hasIntrinsicsCapture(Scene scene);
	bool hasExtrinsicsCapture(Scene scene);

	// Check processed
	bool hasProcessedIntrinsics(Scene scene);
	bool hasProcessedExtrinsics(Scene scene);

	// Delete folders
	void deleteSceneCapture(Scene scene);
	void deleteIntrinsicsCapture(Scene scene);
	void deleteExtrinsicsCapture(Scene scene);
};
