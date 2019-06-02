#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "model/scene/Scene.h"
#include "model/operation/Operation.h"
#include "model/camera/capture/Capture.h"
#include "model/calibration/parameters/Intrinsics.h"
#include "model/calibration/parameters/Extrinsics.h"

using namespace std;
using namespace boost;

class SceneController
{
public:
	SceneController(string dataFolder);
	bool hasScene(string name);
	Scene getScene(string name);
	Scene saveScene(string name);
	bool hasCapture(Scene scene, Operation operation);
	void saveCapture(Scene scene, Operation operation, Capture* capture);
	vector<int> getCapturedCameras(Scene scene, Operation operation);
	int getCapturedFrameNumber(Scene scene, Operation operation);
	bool hasCapturedFrame(Scene scene, Operation operation, int cameraNumber, int frameNumber);
	Mat getCapturedFrame(Scene scene, Operation operation, int cameraNumber, int frameNumber);
	Intrinsics* getIntrinsics(Scene scene, int cameraNumber);
	Extrinsics* getExtrinsics(Scene scene, int cameraNumber);
	void saveIntrinsics(Scene scene, map<int, Intrinsics*> calibrationResults);
	void saveExtrinsics(Scene scene, map<int, Extrinsics*> extrinsicMatrices);
	void saveCalibrationDetections(Mat detection, Scene scene, Operation operation, int cameraNumber, int frameNumber);
private:
	string getDateString();
	string dataFolder;	
};
