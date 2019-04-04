#pragma once

#include <string>
#include "boost/filesystem.h"
#include "model/scene/Scene.h"
#include "model/scene/enum/Operation.h"

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
	bool hasCapture(Scene scene, Operation operation);

	// Check processed
	bool hasProcessed(Scene scene, Operation operation);

	// Delete folders
	void deleteCapture(Scene scene, Operation operation);
};
