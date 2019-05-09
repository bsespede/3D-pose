#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "model/scene/Scene.h"
#include "model/scene/enum/Operation.h"
#include "model/capture/Capture.h"
#include "model/calibration/IntrinsicCalibration.h"

using namespace std;
using namespace boost;

class SceneController
{
public:
	SceneController(string path);

	// I/O scenes
	bool sceneExists(string name);
	Scene loadScene(string name);
	Scene createScene(string name);

	// Capture
	void saveCapture(Scene scene, Operation operation, Capture* capture);
	
	// Process
	vector<string> getCaptureFolders(Scene scene, Operation operation);
	void dumpIntrinsics(vector<IntrinsicCalibration> intrinsicMatrices);

private:
	string path;
};
