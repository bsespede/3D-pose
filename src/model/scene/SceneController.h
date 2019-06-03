#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "model/scene/Scene.h"
#include "model/enum/CaptureType.h"
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
	bool hasCapture(Scene scene, CaptureType captureType);
	void saveCapture(Scene scene, CaptureType captureType, Capture* capture);
	vector<int> getCapturedCameras(Scene scene, CaptureType captureType);
	int getCapturedFrameNumber(Scene scene, CaptureType captureType);
	bool hasCapturedFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber);
	Mat getCapturedFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber);
	Intrinsics* getIntrinsics(Scene scene, int cameraNumber);
	Extrinsics* getExtrinsics(Scene scene, int cameraNumber);
	void saveIntrinsics(Scene scene, map<int, Intrinsics*> intrinsics);
	void saveExtrinsics(Scene scene, map<int, Extrinsics*> extrinsics);
private:
	string getDateString();
	string dataFolder;	
};
