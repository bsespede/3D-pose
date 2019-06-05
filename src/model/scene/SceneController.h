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

class SceneController
{
public:
	SceneController(std::string dataFolder);
	bool hasScene(std::string name);
	Scene getScene(std::string name);
	Scene saveScene(std::string name);
	bool hasCapture(Scene scene, CaptureType captureType);
	void saveCapture(Scene scene, CaptureType captureType, Capture* capture);
	std::vector<int> getCapturedCameras(Scene scene, CaptureType captureType);
	int getCapturedFrames(Scene scene, CaptureType captureType);
	bool hasFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber);
	cv::Mat getFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber);
	void saveIntrinsics(Scene scene, std::map<int, Intrinsics*> intrinsics);
	void saveExtrinsics(Scene scene, std::map<int, Extrinsics*> extrinsics);
	void savePoses(Scene scene, CaptureType captureType, std::vector<Frame3D*> poses);
	std::map<int, Intrinsics*> getIntrinsics(Scene scene);
	std::map<int, Extrinsics*> getExtrinsics(Scene scene);
	std::vector<Frame3D*> getPoses(Scene scene, CaptureType captureType);
	Video3D* getResult(Scene scene, CaptureType captureType);
private:
	std::string getDateString();
	std::string dataFolder;	
};
