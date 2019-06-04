#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "model/scene/Scene.h"
#include "model/capture/CaptureType.h"
#include "model/capture/Capture.h"
#include "model/video/Video3D.h"

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
	int getCapturedFrames(Scene scene, CaptureType captureType);
	bool hasFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber);
	Mat getFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber);
	void saveIntrinsics(Scene scene, map<int, Intrinsics*> intrinsics);
	void saveExtrinsics(Scene scene, map<int, Extrinsics*> extrinsics);
	void savePoses(Scene scene, CaptureType captureType, vector<Frame3D*> poses);
	map<int, Intrinsics*> getIntrinsics(Scene scene);	
	map<int, Extrinsics*> getExtrinsics(Scene scene);	
	vector<Frame3D*> getPoses(Scene scene, CaptureType captureType);
	Video3D* getResult(Scene scene, CaptureType captureType);
private:
	string getDateString();
	string dataFolder;	
};
