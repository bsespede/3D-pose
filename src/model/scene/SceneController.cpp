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
	filesystem::create_directory(operationFolder);

	string captureFile = operationFolder + "/capture.json";
	property_tree::ptree root;

	string date = getDateString();
	root.put("capture.date", date);

	int frameNumber = 0;
	set<int> cameras;
	list<Packet*> packets = capture->getPackets();
	for (Packet* packet: packets)
	{
		for (pair<int, Mat> pair: packet->getData())
		{
			cameras.insert(pair.first);
			string camFolder = operationFolder + "/cam-" + to_string(pair.first);
			filesystem::create_directory(camFolder);

			string frameFolder = camFolder + "/" + to_string(frameNumber) + ".png";
			imwrite(frameFolder, pair.second);			
		}

		frameNumber++;
	}

	delete capture;

	property_tree::ptree camerasNode;
	for (int cameraIndex: cameras)
	{
		property_tree::ptree cameraNode;
		cameraNode.put("", cameraIndex);
		camerasNode.push_back(std::make_pair("", cameraNode));
	}

	root.add_child("capture.cameras", camerasNode);
	root.put("capture.frames", frameNumber);

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

int SceneController::getCapturedFrames(Scene scene, CaptureType captureType)
{
	string captureFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	property_tree::ptree root;
	property_tree::read_json(captureFolder + "/capture.json", root);

	return root.get<int>("capture.frames");
}

bool SceneController::hasFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber)
{
	string frameFile = dataFolder + "/" + scene.getName() + "/" + captureType.toString() + "/cam-" + to_string(cameraNumber) + "/" + to_string(frameNumber) + ".png";
	return filesystem::exists(frameFile);
}

Mat SceneController::getFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber)
{
	string frameFile = dataFolder + "/" + scene.getName() + "/" + captureType.toString() + "/cam-" + to_string(cameraNumber) + "/" + to_string(frameNumber) + ".png";
	return imread(frameFile);
}

void SceneController::saveIntrinsics(Scene scene, map<int, Intrinsics*> intrinsics)
{
	string intrinsicsFile = dataFolder + "/" + scene.getName() + "/intrinsics.json";

	property_tree::ptree root;
	property_tree::ptree camerasNode;

	string date = getDateString();
	root.put("calibration.date", date);

	for (pair<int, Intrinsics*> calibrationResult : intrinsics)
	{
		property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		Intrinsics* intrinsics = calibrationResult.second;

		cameraNode.put("cameraNumber", cameraNumber);
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
	property_tree::ptree camerasNode;

	string date = getDateString();
	root.put("calibration.date", date);

	for (pair<int, Extrinsics*> calibrationResult : extrinsics)
	{
		property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		Extrinsics* extrinsics = calibrationResult.second;

		cameraNode.put("cameraNumber", cameraNumber);
		cameraNode.put("reprojectionError", extrinsics->getReprojectionError());

		Mat translationVector = extrinsics->getTranslationVector();
		cameraNode.put("translationVector.x", translationVector.at<double>(0, 0));
		cameraNode.put("translationVector.y", translationVector.at<double>(1, 0));
		cameraNode.put("translationVector.z", translationVector.at<double>(2, 0));

		Mat rotationVector = extrinsics->getRotationVector();
		cameraNode.put("rotationVector.x", rotationVector.at<double>(0, 0));
		cameraNode.put("rotationVector.y", rotationVector.at<double>(1, 0));
		cameraNode.put("rotationVector.z", rotationVector.at<double>(2, 0));

		camerasNode.push_back(make_pair("", cameraNode));
		delete extrinsics;
	}

	root.add_child("calibration.cameras", camerasNode);
	property_tree::write_json(extrinsicsFile, root);
}


void SceneController::savePoses(Scene scene, CaptureType captureType, vector<Frame3D*> poses)
{
	string extrinsicsFile = dataFolder + "/" + scene.getName() + "/poses.json";
	property_tree::ptree root;

	string date = getDateString();
	root.put("poses.date", date);

	property_tree::ptree allFramesNode;
	for (int frameNumber = 0; frameNumber < poses.size(); frameNumber++)
	{
		Frame3D* frame = poses[frameNumber];

		if (frame != nullptr)
		{
			property_tree::ptree frameDataNode;
			for (pair<int, list<Point3d>> frameData : frame->getData())
			{
				property_tree::ptree cameraNode;
				cameraNode.put("cameraNumber", frameData.first);

				property_tree::ptree cameraPosesNode;
				for (Point3d point : frameData.second)
				{
					property_tree::ptree cameraPoseNode;
					cameraPoseNode.put("x", point.x);
					cameraPoseNode.put("y", point.y);
					cameraPoseNode.put("z", point.z);
					cameraPosesNode.push_back(make_pair("", cameraPoseNode));
				}
				cameraNode.add_child("points", cameraPosesNode);

				frameDataNode.push_back(make_pair("", cameraNode));
			}

			property_tree::ptree frameNode;
			frameNode.put("frameNumber", frameNumber);
			frameNode.add_child("frameData", frameDataNode);
			allFramesNode.push_back(make_pair("", frameNode));
		}
	}

	root.add_child("poses.frames", allFramesNode);
	property_tree::write_json(extrinsicsFile, root);
}

map<int, Intrinsics*> SceneController::getIntrinsics(Scene scene)
{
	map<int, Intrinsics*> intrinsics;
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
		int cameraNumber = cameraNode.second.get<int>("cameraNumber");
		double reprojectionError = cameraNode.second.get<double>("reprojectionError");

		Mat calibrationMatrix = Mat(3, 3, CV_64F);
		calibrationMatrix.at<double>(0, 0) = cameraNode.second.get<double>("calibrationMatrix.fx");
		calibrationMatrix.at<double>(1, 1) = cameraNode.second.get<double>("calibrationMatrix.fy");
		calibrationMatrix.at<double>(0, 2) = cameraNode.second.get<double>("calibrationMatrix.cx");
		calibrationMatrix.at<double>(1, 2) = cameraNode.second.get<double>("calibrationMatrix.cy");

		Mat distortionCoefficients = Mat(1, 5, CV_64F);
		distortionCoefficients.at<double>(0, 0) = cameraNode.second.get<double>("distortionCoefficients.k1");
		distortionCoefficients.at<double>(0, 1) = cameraNode.second.get<double>("distortionCoefficients.k2");
		distortionCoefficients.at<double>(0, 2) = cameraNode.second.get<double>("distortionCoefficients.p1");
		distortionCoefficients.at<double>(0, 3) = cameraNode.second.get<double>("distortionCoefficients.p2");
		distortionCoefficients.at<double>(0, 4) = cameraNode.second.get<double>("distortionCoefficients.k3");

		intrinsics[cameraNumber] = new Intrinsics(calibrationMatrix, distortionCoefficients, reprojectionError);
	}

	return intrinsics;
}

map<int, Extrinsics*> SceneController::getExtrinsics(Scene scene)
{
	map<int, Extrinsics*> extrinsics;
	string extrinsicsFile = dataFolder + "/" + scene.getName() + "/extrinsics.json";

	property_tree::ptree root;
	property_tree::read_json(extrinsicsFile, root);

	for (property_tree::ptree::value_type& cameraNode : root.get_child("calibration.cameras"))
	{
		int cameraNumber = cameraNode.second.get<int>("cameraNumber");

		double reprojectionError = cameraNode.second.get<double>("reprojectionError");

		Mat translationVector = Mat(3, 1, CV_64F);
		translationVector.at<double>(0, 0) = cameraNode.second.get<double>("translationVector.x");
		translationVector.at<double>(1, 0) = cameraNode.second.get<double>("translationVector.y");
		translationVector.at<double>(2, 0) = cameraNode.second.get<double>("translationVector.z");

		Mat rotationVector = Mat(3, 1, CV_64F);
		rotationVector.at<double>(0, 0) = cameraNode.second.get<double>("rotationVector.x");
		rotationVector.at<double>(1, 0) = cameraNode.second.get<double>("rotationVector.y");
		rotationVector.at<double>(2, 0) = cameraNode.second.get<double>("rotationVector.z");

		extrinsics[cameraNumber] = new Extrinsics(translationVector, rotationVector, reprojectionError);
	}

	return extrinsics;
}

vector<Frame3D*> SceneController::getPoses(Scene scene, CaptureType captureType)
{
	int capturedFrames = getCapturedFrames(scene, captureType);
	vector<Frame3D*> allFrames = vector<Frame3D*>(capturedFrames, nullptr);

	string posesFile = dataFolder + "/" + scene.getName() + "/poses.json";

	property_tree::ptree root;
	property_tree::read_json(posesFile, root);
	if (!filesystem::exists(posesFile))
	{
		return allFrames;
	}	

	for (property_tree::ptree::value_type& frameNode : root.get_child("poses.frames"))
	{
		Frame3D* frame = new Frame3D();
		int frameNumber = frameNode.second.get<int>("frameNumber");

		for (property_tree::ptree::value_type& dataNode : frameNode.second.get_child("frameData"))
		{
			list<Point3d> reconstructions;
			int cameraNumber = dataNode.second.get<int>("cameraNumber");

			for (property_tree::ptree::value_type& pointNode : dataNode.second.get_child("points"))
			{
				Point3d point;
				point.x = pointNode.second.get<double>("x");
				point.y = pointNode.second.get<double>("y");
				point.z = pointNode.second.get<double>("z");

				reconstructions.push_back(point);
			}

			frame->addData(cameraNumber, reconstructions);
		}

		allFrames[frameNumber] = frame;
	}

	return allFrames;
}

Video3D* SceneController::getResult(Scene scene, CaptureType captureType)
{
	vector<int> capturedCameras = getCapturedCameras(scene, captureType);
	int capturedFrames = getCapturedFrames(scene, captureType);

	map<int, Intrinsics*> intrinsics = getIntrinsics(scene);
	map<int, Extrinsics*> extrinsics = getExtrinsics(scene);
	map<int, Mat> frustumImages;

	string extrinsicsFolder = dataFolder + "/" + scene.getName() + "/extrinsics.json";
	if (!filesystem::exists(extrinsicsFolder))
	{
		return nullptr;
	}
	
	for (int cameraNumber : capturedCameras)
	{
		frustumImages[cameraNumber] = getFrame(scene, captureType, cameraNumber, capturedFrames - 1);
	}

	vector<Frame3D*> poses = getPoses(scene, captureType);
	
	return new Video3D(capturedCameras, intrinsics, extrinsics, frustumImages);
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