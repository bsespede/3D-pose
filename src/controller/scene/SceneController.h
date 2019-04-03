#pragma once

#include <string>
#include "boost/filesystem.h"
#include "model/scene/Scene.h"

using namespace std;
using namespace boost;

class SceneController
{
private:
	string path;
public:
	SceneController(string path);

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
