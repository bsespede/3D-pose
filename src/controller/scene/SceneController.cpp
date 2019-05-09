#include "SceneController.h"

SceneController::SceneController(string path)
{
	this->path = path;
}

bool SceneController::sceneExists(string name)
{
	return filesystem::exists(path + "/" + name + "/scene.json");
}

Scene SceneController::loadScene(string name)
{
	string scenePath = path + "/" + name;

	property_tree::ptree root;
	property_tree::read_json(scenePath + "/scene.json", root);
	
	string sceneName = root.get<string>("scene.name");
	string sceneDate = root.get<string>("scene.date");

	return Scene(sceneName, sceneDate);
}

Scene SceneController::createScene(string name)
{
	string scenePath = path + "/" + name;
	filesystem::create_directory(scenePath);

	time_t _tm = time(NULL);
	struct tm * curtime = localtime(&_tm);
	string date = asctime(curtime);

	string configFile = scenePath + "/scene.json";
	property_tree::ptree root;

	root.put("scene.name", name);
	root.put("scene.date", date);

	property_tree::write_json(configFile, root);
	return Scene(name, date);
}

bool SceneController::hasCapture(Scene scene, Operation operation)
{
	string operationPath = path + "/" + scene.getName() + "/" + operation.toString();
	return filesystem::exists(operationPath + "/capture.json");
}

void SceneController::saveCapture(Scene scene, Operation operation, Capture* capture)
{
	string operationPath = path + "/" + scene.getName() + "/" + operation.toString();
	filesystem::create_directory(operationPath);

	list<FramesPacket*> frames = capture->getFrames();
	int frameNumber = 0;
	set<int> cameraSet;

	for (FramesPacket* framePacket: frames)
	{
		for (pair<int, Mat> pair: framePacket->getFrames())
		{
			cameraSet.insert(pair.first);
			string camPath = operationPath + "/cam-" + to_string(pair.first);
			filesystem::create_directory(camPath);

			string framePath = camPath + "/" + to_string(frameNumber) + ".png";
			imwrite(framePath, pair.second);			
		}

		frameNumber++;
	}

	delete capture;

	string recordFile = operationPath + "/capture.json";
	property_tree::ptree root;

	if (filesystem::exists(recordFile))
	{
		property_tree::ptree previousRoot;
		property_tree::read_json(recordFile, previousRoot);	

		for (property_tree::ptree::value_type &camera: previousRoot.get_child("capture.cameras"))
		{
			int cameraNumber = camera.second.get_value<int>();
			cameraSet.insert(cameraNumber);
		}
	}	

	time_t _tm = time(NULL);
	struct tm* curtime = localtime(&_tm);
	string date = asctime(curtime);
	root.put("capture.date", date);

	property_tree::ptree camerasNode;
	for (int cameraIndex: cameraSet)
	{
		property_tree::ptree cameraNode;
		cameraNode.put("", cameraIndex);
		camerasNode.push_back(std::make_pair("", cameraNode));
	}

	root.add_child("capture.cameras", camerasNode);
	root.put("capture.frames", frameNumber);

	property_tree::write_json(recordFile, root);
}

map<int, string> SceneController::getCapturedCameras(Scene scene, Operation operation)
{
	map<int, string> capturedCameras;

	string capturePath = path + "/" + scene.getName() + "/" + operation.toString();
	property_tree::ptree root;
	property_tree::read_json(capturePath + "/capture.json", root);	

	for (property_tree::ptree::value_type &camera: root.get_child("capture.cameras"))
	{
		int cameraNumber = camera.second.get_value<int>();
		capturedCameras[cameraNumber] = capturePath + "/cam-" + to_string(cameraNumber);
	}

	return capturedCameras;
}

void SceneController::dumpIntrinsics(map<int, IntrinsicCalibration*> calibrationResults)
{
	string configFile = path + "/calibration.json";
	property_tree::ptree root;

	time_t _tm = time(NULL);
	struct tm* curtime = localtime(&_tm);
	string date = asctime(curtime);
	root.put("calibration.date", date);

	property_tree::ptree camerasNode;
	for (pair<int, IntrinsicCalibration*> calibrationResult: calibrationResults)
	{
		property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		IntrinsicCalibration* intrinsics = calibrationResult.second;
		
		cameraNode.put("cameraId", cameraNumber);
		cameraNode.put("reprojectionError", intrinsics->getReprojectionError());

		Mat cameraMatrix = intrinsics->getCameraMatrix();
		cameraNode.put("calibrationMatrix.fx", cameraMatrix.at<double>(0, 0));
		cameraNode.put("calibrationMatrix.fy", cameraMatrix.at<double>(1, 1));
		cameraNode.put("calibrationMatrix.cx", cameraMatrix.at<double>(0, 2));
		cameraNode.put("calibrationMatrix.cy", cameraMatrix.at<double>(1, 2));

		Mat distortionCoeffs = intrinsics->getDistortionCoeffs();
		cameraNode.put("distortionCoefficients.k1", distortionCoeffs.at<double>(0, 0));
		cameraNode.put("distortionCoefficients.k2", distortionCoeffs.at<double>(0, 1));
		cameraNode.put("distortionCoefficients.p1", distortionCoeffs.at<double>(0, 2));
		cameraNode.put("distortionCoefficients.p2", distortionCoeffs.at<double>(0, 3));
		cameraNode.put("distortionCoefficients.k3", distortionCoeffs.at<double>(0, 4));

		camerasNode.push_back(std::make_pair("", cameraNode));
		delete intrinsics;
	}

	root.add_child("calibration.cameras", camerasNode);
	
	property_tree::write_json(configFile, root);
}