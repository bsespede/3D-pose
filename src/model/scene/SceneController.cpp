#include "SceneController.h"

SceneController::SceneController(std::string dataFolder)
{
	this->dataFolder = dataFolder;
}

bool SceneController::hasScene(std::string name)
{
	return boost::filesystem::exists(dataFolder + "/" + name + "/scene.json");
}

Scene SceneController::getScene(std::string name)
{
	std::string sceneFolder = dataFolder + "/" + name;

	boost::property_tree::ptree root;
	boost::property_tree::read_json(sceneFolder + "/scene.json", root);
	
	std::string sceneName = root.get<std::string>("scene.name");
	std::string sceneDate = root.get<std::string>("scene.date");

	return Scene(sceneName, sceneDate);
}

Scene SceneController::saveScene(std::string name)
{
	std::string sceneFolder = dataFolder + "/" + name;
	boost::filesystem::create_directory(sceneFolder);

	std::string configFile = sceneFolder + "/scene.json";
	boost::property_tree::ptree root;

	std::string date = getDateString();
	root.put("scene.name", name);
	root.put("scene.date", date);

	boost::property_tree::write_json(configFile, root);
	return Scene(name, date);
}

bool SceneController::hasCapture(Scene scene, CaptureType captureType)
{
	std::string operationFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	return boost::filesystem::exists(operationFolder + "/capture.json");
}

void SceneController::saveCapture(Scene scene, CaptureType captureType, Capture* capture)
{
	std::string operationFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	boost::filesystem::create_directory(operationFolder);

	std::string captureFile = operationFolder + "/capture.json";
	boost::property_tree::ptree root;

	std::string date = getDateString();
	root.put("capture.date", date);

	int frameNumber = 0;
	std::set<int> cameras;
	std::list<Packet*> packets = capture->getPackets();
	for (Packet* packet: packets)
	{
		for (std::pair<int, cv::Mat> pair: packet->getData())
		{
			cameras.insert(pair.first);
			std::string camFolder = operationFolder + "/cam-" + std::to_string(pair.first);
			boost::filesystem::create_directory(camFolder);

			std::string frameFolder = camFolder + "/" + std::to_string(frameNumber) + ".png";
			cv::imwrite(frameFolder, pair.second);			
		}

		frameNumber++;
	}

	delete capture;

	boost::property_tree::ptree camerasNode;
	for (int cameraIndex: cameras)
	{
		boost::property_tree::ptree cameraNode;
		cameraNode.put("", cameraIndex);
		camerasNode.push_back(std::make_pair("", cameraNode));
	}

	root.add_child("capture.cameras", camerasNode);
	root.put("capture.frames", frameNumber);

	boost::property_tree::write_json(captureFile, root);
}

std::vector<int> SceneController::getCapturedCameras(Scene scene, CaptureType captureType)
{
	std::vector<int> capturedCameras;

	std::string captureFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	boost::property_tree::ptree root;
	boost::property_tree::read_json(captureFolder + "/capture.json", root);	

	for (boost::property_tree::ptree::value_type &camera: root.get_child("capture.cameras"))
	{
		capturedCameras.push_back(camera.second.get_value<int>());
	}

	return capturedCameras;
}

int SceneController::getCapturedFrames(Scene scene, CaptureType captureType)
{
	std::string captureFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString();
	boost::property_tree::ptree root;
	boost::property_tree::read_json(captureFolder + "/capture.json", root);

	return root.get<int>("capture.frames");
}

bool SceneController::hasFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber)
{
	std::string frameFile = dataFolder + "/" + scene.getName() + "/" + captureType.toString() + "/cam-" + std::to_string(cameraNumber) + "/" + std::to_string(frameNumber) + ".png";
	return boost::filesystem::exists(frameFile);
}

cv::Mat SceneController::getFrame(Scene scene, CaptureType captureType, int cameraNumber, int frameNumber)
{
	std::string frameFile = dataFolder + "/" + scene.getName() + "/" + captureType.toString() + "/cam-" + std::to_string(cameraNumber) + "/" + std::to_string(frameNumber) + ".png";
	return cv::imread(frameFile);
}

void SceneController::saveIntrinsics(Scene scene, std::map<int, Intrinsics*> intrinsics)
{
	CalibrationType calibrationType = CalibrationType::INTRINSICS;
	boost::property_tree::ptree root;
	boost::property_tree::ptree camerasNode;

	std::string date = getDateString();
	root.put("calibration.date", date);

	for (std::pair<int, Intrinsics*> calibrationResult : intrinsics)
	{
		boost::property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		Intrinsics* intrinsics = calibrationResult.second;

		cameraNode.put("cameraNumber", cameraNumber);
		cameraNode.put("reprojectionError", intrinsics->getReprojectionError());

		cv::Mat cameraMatrix = intrinsics->getCameraMatrix();
		cameraNode.put("calibrationMatrix.fx", cameraMatrix.at<double>(0, 0));
		cameraNode.put("calibrationMatrix.fy", cameraMatrix.at<double>(1, 1));
		cameraNode.put("calibrationMatrix.cx", cameraMatrix.at<double>(0, 2));
		cameraNode.put("calibrationMatrix.cy", cameraMatrix.at<double>(1, 2));

		cv::Mat distortionCoefficients = intrinsics->getDistortionCoefficients();
		cameraNode.put("distortionCoefficients.k1", distortionCoefficients.at<double>(0, 0));
		cameraNode.put("distortionCoefficients.k2", distortionCoefficients.at<double>(0, 1));
		cameraNode.put("distortionCoefficients.p1", distortionCoefficients.at<double>(0, 2));
		cameraNode.put("distortionCoefficients.p2", distortionCoefficients.at<double>(0, 3));
		cameraNode.put("distortionCoefficients.k3", distortionCoefficients.at<double>(0, 4));

		camerasNode.push_back(make_pair("", cameraNode));
		delete intrinsics;
	}

	root.add_child("calibration.cameras", camerasNode);

	std::string intrinsicsFile = dataFolder + "/" + scene.getName() + "/" + calibrationType.toString() + ".json";
	boost::property_tree::write_json(intrinsicsFile, root);
}

void SceneController::saveExtrinsics(Scene scene, std::map<int, Extrinsics*> extrinsics)
{
	CalibrationType calibrationType = CalibrationType::EXTRINSICS;
	boost::property_tree::ptree root;
	boost::property_tree::ptree camerasNode;

	std::string date = getDateString();
	root.put("calibration.date", date);

	for (std::pair<int, Extrinsics*> calibrationResult : extrinsics)
	{
		boost::property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		Extrinsics* extrinsics = calibrationResult.second;

		cameraNode.put("cameraNumber", cameraNumber);
		cameraNode.put("reprojectionError", extrinsics->getReprojectionError());

		cv::Mat translationVector = extrinsics->getTranslationVector();
		cameraNode.put("translationVector.x", translationVector.at<double>(0, 0));
		cameraNode.put("translationVector.y", translationVector.at<double>(1, 0));
		cameraNode.put("translationVector.z", translationVector.at<double>(2, 0));

		cv::Mat rotationVector = extrinsics->getRotationVector();
		cameraNode.put("rotationVector.x", rotationVector.at<double>(0, 0));
		cameraNode.put("rotationVector.y", rotationVector.at<double>(1, 0));
		cameraNode.put("rotationVector.z", rotationVector.at<double>(2, 0));

		camerasNode.push_back(make_pair("", cameraNode));
		delete extrinsics;
	}

	root.add_child("calibration.cameras", camerasNode);

	std::string extrinsicsFile = dataFolder + "/" + scene.getName() + "/" + calibrationType.toString() + ".json";
	boost::property_tree::write_json(extrinsicsFile, root);
}


void SceneController::savePoses(Scene scene, CaptureType captureType, std::vector<Frame3D*> poses)
{
	CalibrationType calibrationType = CalibrationType::POSES;
	boost::property_tree::ptree root;

	std::string date = getDateString();
	root.put("poses.date", date);

	boost::property_tree::ptree allFramesNode;
	for (int frameNumber = 0; frameNumber < poses.size(); frameNumber++)
	{
		Frame3D* frame = poses[frameNumber];

		if (frame != nullptr)
		{
			boost::property_tree::ptree frameDataNode;
			for (std::pair<int, std::list<cv::Point3d>> frameData : frame->getData())
			{
				boost::property_tree::ptree cameraNode;
				cameraNode.put("cameraNumber", frameData.first);

				boost::property_tree::ptree cameraPosesNode;
				for (cv::Point3d point : frameData.second)
				{
					boost::property_tree::ptree cameraPoseNode;
					cameraPoseNode.put("x", point.x);
					cameraPoseNode.put("y", point.y);
					cameraPoseNode.put("z", point.z);
					cameraPosesNode.push_back(make_pair("", cameraPoseNode));
				}
				cameraNode.add_child("points", cameraPosesNode);

				frameDataNode.push_back(make_pair("", cameraNode));
			}

			boost::property_tree::ptree frameNode;
			frameNode.put("frameNumber", frameNumber);
			frameNode.add_child("frameData", frameDataNode);
			allFramesNode.push_back(make_pair("", frameNode));
		}
	}

	root.add_child("poses.frames", allFramesNode);

	std::string extrinsicsFile = dataFolder + "/" + scene.getName() + "/" + calibrationType.toString() + ".json";
	boost::property_tree::write_json(extrinsicsFile, root);
}

void SceneController::saveDetections(cv::Mat output, Scene scene, CaptureType captureType, int frameNumber, CalibrationType calibrationType)
{
	std::string outputFolder = dataFolder + "/" + scene.getName() + "/" + captureType.toString() + "/" + calibrationType.toString();

	if (!boost::filesystem::exists(outputFolder))
	{
		boost::filesystem::create_directory(outputFolder);
	}

	std::string fileName = outputFolder + "/" + std::to_string(frameNumber) + ".png";
	cv::imwrite(fileName, output);
}

std::map<int, Intrinsics*> SceneController::getIntrinsics(Scene scene)
{
	CalibrationType calibrationType = CalibrationType::INTRINSICS;

	std::map<int, Intrinsics*> intrinsics;
	std::string intrinsicsFile = dataFolder + "/" + scene.getName() + "/" + calibrationType.toString() + ".json";

	if (!boost::filesystem::exists(intrinsicsFile))
	{
		intrinsicsFile = dataFolder + "/default.json";
	}

	boost::property_tree::ptree root;
	boost::property_tree::read_json(intrinsicsFile, root);		
	for (boost::property_tree::ptree::value_type& cameraNode: root.get_child("calibration.cameras"))
	{
		int cameraNumber = cameraNode.second.get<int>("cameraNumber");
		double reprojectionError = cameraNode.second.get<double>("reprojectionError");

		cv::Mat calibrationMatrix = cv::Mat(3, 3, CV_64F);
		calibrationMatrix.at<double>(0, 0) = cameraNode.second.get<double>("calibrationMatrix.fx");
		calibrationMatrix.at<double>(1, 1) = cameraNode.second.get<double>("calibrationMatrix.fy");
		calibrationMatrix.at<double>(0, 2) = cameraNode.second.get<double>("calibrationMatrix.cx");
		calibrationMatrix.at<double>(1, 2) = cameraNode.second.get<double>("calibrationMatrix.cy");

		cv::Mat distortionCoefficients = cv::Mat(1, 5, CV_64F);
		distortionCoefficients.at<double>(0, 0) = cameraNode.second.get<double>("distortionCoefficients.k1");
		distortionCoefficients.at<double>(0, 1) = cameraNode.second.get<double>("distortionCoefficients.k2");
		distortionCoefficients.at<double>(0, 2) = cameraNode.second.get<double>("distortionCoefficients.p1");
		distortionCoefficients.at<double>(0, 3) = cameraNode.second.get<double>("distortionCoefficients.p2");
		distortionCoefficients.at<double>(0, 4) = cameraNode.second.get<double>("distortionCoefficients.k3");

		intrinsics[cameraNumber] = new Intrinsics(calibrationMatrix, distortionCoefficients, reprojectionError);
	}

	return intrinsics;
}

std::map<int, Extrinsics*> SceneController::getExtrinsics(Scene scene)
{
	std::map<int, Extrinsics*> extrinsics;
	std::string extrinsicsFile = dataFolder + "/" + scene.getName() + "/extrinsics.json";

	boost::property_tree::ptree root;
	boost::property_tree::read_json(extrinsicsFile, root);

	for (boost::property_tree::ptree::value_type& cameraNode : root.get_child("calibration.cameras"))
	{
		int cameraNumber = cameraNode.second.get<int>("cameraNumber");

		double reprojectionError = cameraNode.second.get<double>("reprojectionError");

		cv::Mat translationVector = cv::Mat(3, 1, CV_64F);
		translationVector.at<double>(0, 0) = cameraNode.second.get<double>("translationVector.x");
		translationVector.at<double>(1, 0) = cameraNode.second.get<double>("translationVector.y");
		translationVector.at<double>(2, 0) = cameraNode.second.get<double>("translationVector.z");

		cv::Mat rotationVector = cv::Mat(3, 1, CV_64F);
		rotationVector.at<double>(0, 0) = cameraNode.second.get<double>("rotationVector.x");
		rotationVector.at<double>(1, 0) = cameraNode.second.get<double>("rotationVector.y");
		rotationVector.at<double>(2, 0) = cameraNode.second.get<double>("rotationVector.z");

		extrinsics[cameraNumber] = new Extrinsics(translationVector, rotationVector, reprojectionError);
	}

	return extrinsics;
}

std::vector<Frame3D*> SceneController::getPoses(Scene scene, CaptureType captureType)
{
	int capturedFrames = getCapturedFrames(scene, captureType);
	std::vector<Frame3D*> allFrames = std::vector<Frame3D*>(capturedFrames, nullptr);

	std::string posesFile = dataFolder + "/" + scene.getName() + "/poses.json";

	boost::property_tree::ptree root;
	boost::property_tree::read_json(posesFile, root);
	if (!boost::filesystem::exists(posesFile))
	{
		return allFrames;
	}	

	for (boost::property_tree::ptree::value_type& frameNode : root.get_child("poses.frames"))
	{
		Frame3D* frame = new Frame3D();
		int frameNumber = frameNode.second.get<int>("frameNumber");

		for (boost::property_tree::ptree::value_type& dataNode : frameNode.second.get_child("frameData"))
		{
			std::list<cv::Point3d> reconstructions;
			int cameraNumber = dataNode.second.get<int>("cameraNumber");

			for (boost::property_tree::ptree::value_type& pointNode : dataNode.second.get_child("points"))
			{
				cv::Point3d point;
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
	std::string extrinsicsFolder = dataFolder + "/" + scene.getName() + "/extrinsics.json";

	if (!boost::filesystem::exists(extrinsicsFolder))
	{
		return nullptr;
	}
	else
	{
		std::vector<int> capturedCameras = getCapturedCameras(scene, captureType);
		std::map<int, Intrinsics*> intrinsics = getIntrinsics(scene);
		std::map<int, Extrinsics*> extrinsics = getExtrinsics(scene);
		Video3D* result = new Video3D(capturedCameras, intrinsics, extrinsics);

		for (Frame3D* frame3D : getPoses(scene, captureType))
		{
			result->addFrame(frame3D);
		}

		return result;
	}		
}

std::string SceneController::getDateString()
{
	time_t rawTime = time(NULL);
	char buffer[26];
	ctime_s(buffer, sizeof(buffer), &rawTime);
	std::string date = std::string(buffer);
	date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

	return date;
}