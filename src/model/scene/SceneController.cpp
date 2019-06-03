#include "SceneController.h"

SceneController::SceneController(string dataFolder)
{
	this->dataFolder = dataFolder;
}

bool SceneController::hasScene(string name)
{
	return filesystem::exists(dataFolder + "/" + name + "/scene.json");
}

Scene SceneController::getScene(string name)
{
	string sceneFolder = dataFolder + "/" + name;

	property_tree::ptree root;
	property_tree::read_json(sceneFolder + "/scene.json", root);
	
	string sceneName = root.get<string>("scene.name");
	string sceneDate = root.get<string>("scene.date");

	return Scene(sceneName, sceneDate);
}

Scene SceneController::saveScene(string name)
{
	string sceneFolder = dataFolder + "/" + name;
	filesystem::create_directory(sceneFolder);

	string configFile = sceneFolder + "/scene.json";
	property_tree::ptree root;

	string date = getDateString();
	root.put("scene.name", name);
	root.put("scene.date", date);

	property_tree::write_json(configFile, root);
	return Scene(name, date);
}

bool SceneController::hasCapture(Scene scene, CaptureType captureType)
{
	string operationFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	return filesystem::exists(operationFolder + "/capture.json");
}

void SceneController::saveCapture(Scene scene, CaptureType captureType, Capture* capture)
{
	string operationFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	string captureFile = operationFolder + "/capture.json";
	property_tree::ptree root;

	int frameNumber;
	set<int> cameraSet;
	if (filesystem::exists(captureFile))
	{
		property_tree::ptree previousRoot;
		property_tree::read_json(captureFile, previousRoot);
		frameNumber = root.get<int>("capture.frames");
	}
	else
	{
		filesystem::create_directory(operationFolder);
		frameNumber = 0;
	}

	list<Packet*> packets = capture->getPackets();
	for (Packet* packet: packets)
	{
		for (pair<int, Mat> pair: packet->getData())
		{
			cameraSet.insert(pair.first);
			string camFolder = operationFolder + "/cam-" + to_string(pair.first);
			filesystem::create_directory(camFolder);

			string frameFolder = camFolder + "/" + to_string(frameNumber) + ".png";
			imwrite(frameFolder, pair.second);			
		}

		frameNumber++;
	}

	delete capture;

	property_tree::ptree camerasNode;
	for (int cameraIndex: cameraSet)
	{
		property_tree::ptree cameraNode;
		cameraNode.put("", cameraIndex);
		camerasNode.push_back(std::make_pair("", cameraNode));
	}

	root.add_child("capture.cameras", camerasNode);
	root.put("capture.frames", frameNumber);

	string date = getDateString();
	root.put("capture.date", date);

	property_tree::write_json(captureFile, root);
}

vector<int> SceneController::getCapturedCameras(Scene scene, CaptureType captureType)
{
	vector<int> capturedCameras;

	string captureFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	property_tree::ptree root;
	property_tree::read_json(captureFolder + "/capture.json", root);	

	for (property_tree::ptree::value_type &camera: root.get_child("capture.cameras"))
	{
		capturedCameras.push_back(camera.second.get_value<int>());
	}

	return capturedCameras;
}

int SceneController::getCapturedFrameNumber(Scene scene, CaptureType captureType)
{
	string captureFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	property_tree::ptree root;
	property_tree::read_json(captureFolder + "/capture.json", root);

	return root.get<int>("capture.frames");
}

bool SceneController::hasCapturedFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber)
{
	string frameFile = dataFolder + "/" + scene.getName() + "/" + captureType.toString() + "/cam-" + to_string(cameraNumber) + "/" + to_string(frameNumber) + ".png";
	return filesystem::exists(frameFile);
}

Mat SceneController::getCapturedFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber)
{
	string frameFile = dataFolder + "/" + scene.getName() + "/" + captureType.toString() + "/cam-" + to_string(cameraNumber) + "/" + to_string(frameNumber) + ".png";
	Mat frame = imread(frameFile);
	return frame;	
}

Intrinsics* SceneController::getIntrinsics(Scene scene, int cameraNumber)
{
	string captureFolder = dataFolder + "/" + scene.getName();

	string intrinsicsFile = captureFolder + "/intrinsics.json"; 
	if (!filesystem::exists(intrinsicsFile))
	{
		intrinsicsFile = dataFolder + "/default.json";
	}

	property_tree::ptree root;
	property_tree::read_json(intrinsicsFile, root);	
	for (property_tree::ptree::value_type& cameraNode: root.get_child("calibration.cameras"))
	{
		if (cameraNumber == cameraNode.second.get<int>("cameraId")) 
		{
			double reprojectionError = cameraNode.second.get<double>("reprojectionError");
			double fx = cameraNode.second.get<double>("calibrationMatrix.fx");
			double fy = cameraNode.second.get<double>("calibrationMatrix.fy");
			double cx = cameraNode.second.get<double>("calibrationMatrix.cx");
			double cy = cameraNode.second.get<double>("calibrationMatrix.cy");

			Mat calibrationMatrix = Mat(3, 3, CV_64F);
			calibrationMatrix.at<double>(0, 0) = fx;
			calibrationMatrix.at<double>(1, 1) = fy;
			calibrationMatrix.at<double>(0, 2) = cx;
			calibrationMatrix.at<double>(1, 2) = cy;

			double k1 = cameraNode.second.get<double>("distortionCoefficients.k1");
			double k2 = cameraNode.second.get<double>("distortionCoefficients.k2");
			double p1 = cameraNode.second.get<double>("distortionCoefficients.p1");
			double p2 = cameraNode.second.get<double>("distortionCoefficients.p2");
			double k3 = cameraNode.second.get<double>("distortionCoefficients.k3");

			Mat distortionCoefficients = Mat(1, 5, CV_64F);
			distortionCoefficients.at<double>(0, 0) = k1;
			distortionCoefficients.at<double>(0, 1) = k2;
			distortionCoefficients.at<double>(0, 2) = p1;
			distortionCoefficients.at<double>(0, 3) = p2;
			distortionCoefficients.at<double>(0, 4) = k3;

			Intrinsics* intrinsics = new Intrinsics(calibrationMatrix, distortionCoefficients, reprojectionError);
			return intrinsics;
		}
	}

	return nullptr;
}

Extrinsics* SceneController::getExtrinsics(Scene scene, int cameraNumber)
{
	string extrinsicsFile = dataFolder + "/" + scene.getName() + "/extrinsics.json";
	property_tree::ptree root;
	property_tree::read_json(extrinsicsFile, root);

	for (property_tree::ptree::value_type& cameraNode : root.get_child("calibration.cameras"))
	{
		if (cameraNumber == cameraNode.second.get<int>("cameraId"))
		{
			double reprojectionError = cameraNode.second.get<double>("reprojectionError");

			double translationX = cameraNode.second.get<double>("translationMatrix.x");
			double translationY = cameraNode.second.get<double>("translationMatrix.y");
			double translationZ = cameraNode.second.get<double>("translationMatrix.z");

			Mat translationVector = Mat(3, 1, CV_64F);
			translationVector.at<double>(0, 0) = translationX;
			translationVector.at<double>(1, 0) = translationY;
			translationVector.at<double>(2, 0) = translationZ;

			double rotationX = cameraNode.second.get<double>("rotationMatrix.x");
			double rotationY = cameraNode.second.get<double>("rotationMatrix.y");
			double rotationZ = cameraNode.second.get<double>("rotationMatrix.z");

			Mat rotationVector = Mat(3, 1, CV_64F);
			rotationVector.at<double>(0, 0) = rotationX;
			rotationVector.at<double>(1, 0) = rotationY;
			rotationVector.at<double>(2, 0) = rotationZ;

			Extrinsics* extrinsics = new Extrinsics(translationVector, rotationVector, reprojectionError);
			return extrinsics;
		}
	}

	return nullptr;
}

void SceneController::saveIntrinsics(Scene scene, map<int, Intrinsics*> intrinsics)
{
	string intrinsicsFile = dataFolder + "/" + scene.getName() + "/intrinsics.json";
	property_tree::ptree root;

	string date = getDateString();
	root.put("calibration.date", date);

	property_tree::ptree camerasNode;
	for (pair<int, Intrinsics*> calibrationResult: intrinsics)
	{
		property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		Intrinsics* intrinsics = calibrationResult.second;
		
		cameraNode.put("cameraId", cameraNumber);
		cameraNode.put("reprojectionError", intrinsics->getReprojectionError());

		Mat cameraMatrix = intrinsics->getCameraMatrix();
		cameraNode.put("calibrationMatrix.fx", cameraMatrix.at<double>(0, 0));
		cameraNode.put("calibrationMatrix.fy", cameraMatrix.at<double>(1, 1));
		cameraNode.put("calibrationMatrix.cx", cameraMatrix.at<double>(0, 2));
		cameraNode.put("calibrationMatrix.cy", cameraMatrix.at<double>(1, 2));

		Mat distortionCoefficients = intrinsics->getDistortionCoefficients();
		cameraNode.put("distortionCoefficients.k1", distortionCoefficients.at<double>(0, 0));
		cameraNode.put("distortionCoefficients.k2", distortionCoefficients.at<double>(0, 1));
		cameraNode.put("distortionCoefficients.p1", distortionCoefficients.at<double>(0, 2));
		cameraNode.put("distortionCoefficients.p2", distortionCoefficients.at<double>(0, 3));
		cameraNode.put("distortionCoefficients.k3", distortionCoefficients.at<double>(0, 4));

		camerasNode.push_back(make_pair("", cameraNode));
		delete intrinsics;
	}

	root.add_child("calibration.cameras", camerasNode);	
	property_tree::write_json(intrinsicsFile, root);
}

void SceneController::saveExtrinsics(Scene scene, map<int, Extrinsics*> extrinsics)
{
	string extrinsicsFile = dataFolder + "/" + scene.getName() + "/extrinsics.json";
	property_tree::ptree root;

	string date = getDateString();
	root.put("calibration.date", date);

	property_tree::ptree camerasNode;
	for (pair<int, Extrinsics*> calibrationResult: extrinsics)
	{
		property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		Extrinsics* extrinsics = calibrationResult.second;
		
		cameraNode.put("cameraId", cameraNumber);
		cameraNode.put("reprojectionError", extrinsics->getReprojectionError());

		Mat translationVector = extrinsics->getTranslationVector();
		cameraNode.put("translationMatrix.x", translationVector.at<double>(0, 0));
		cameraNode.put("translationMatrix.y", translationVector.at<double>(1, 0));
		cameraNode.put("translationMatrix.z", translationVector.at<double>(2, 0));

		Mat rotationVector = extrinsics->getRotationVector();
		cameraNode.put("rotationMatrix.x", rotationVector.at<double>(0, 0));
		cameraNode.put("rotationMatrix.y", rotationVector.at<double>(1, 0));
		cameraNode.put("rotationMatrix.z", rotationVector.at<double>(2, 0));

		camerasNode.push_back(make_pair("", cameraNode));
		delete extrinsics;
	}

	root.add_child("calibration.cameras", camerasNode);	
	property_tree::write_json(extrinsicsFile, root);
}

string SceneController::getDateString()
{
	time_t rawTime = time(NULL);
	char buffer[26];
	ctime_s(buffer, sizeof(buffer), &rawTime);
	string date = string(buffer);
	date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

	return date;
}