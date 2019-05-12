#include "FileController.h"

FileController::FileController()
{
	property_tree::ptree root;
	property_tree::read_json("app-config.json", root);

	this->dataFolder = root.get<string>("config.dataFolder");

	this->maxWidth = root.get<int>("config.gui.maxWidth");
	this->maxHeight = root.get<int>("config.gui.maxHeight");
	this->cameraHeight = root.get<int>("config.gui.cameraHeight");
	this->cameraWidth = root.get<int>("config.gui.cameraWidth");
	this->barHeight = root.get<int>("config.gui.barHeight");
	this->guiFps = root.get<int>("config.gui.fps");
	this->showPreviewOnCapture = root.get<bool>("config.gui.showCapturePreview");

	int cameraNumber = 0;
	this->camerasOrder = std::map<int, int>();
	this->camerasFps = root.get<int>("config.cameras.fps");
	for (property_tree::ptree::value_type& cameraId : root.get_child("config.cameras.order"))
	{
		int serial = cameraId.second.get_value<int>();
		camerasOrder[serial] = cameraNumber;
		cameraNumber++;
	}

	this->maxCheckboards = root.get<int>("config.calibration.checkboardIntrinsics.frames");
	this->checkboardTimer = root.get<int>("config.calibration.checkboardIntrinsics.timer");

	this->checkboardCols = root.get<int>("config.calibration.checkboardIntrinsics.cols");
	this->checkboardRows = root.get<int>("config.calibration.checkboardIntrinsics.rows");
	this->checkboardSquareLength = root.get<float>("config.calibration.checkboardIntrinsics.squareLength");
	this->checkboardMarkerLength = root.get<float>("config.calibration.checkboardIntrinsics.markerLength");
	this->checkboardWidth = root.get<int>("config.calibration.checkboardImage.width");
	this->checkboardHeight = root.get<int>("config.calibration.checkboardImage.height");
	this->checkboardMargin = root.get<int>("config.calibration.checkboardImage.margin");
}

bool FileController::sceneExists(string name)
{
	return filesystem::exists(dataFolder + "/" + name + "/scene.json");
}

Scene FileController::loadScene(string name)
{
	string scenePath = dataFolder + "/" + name;

	property_tree::ptree root;
	property_tree::read_json(scenePath + "/scene.json", root);
	
	string sceneName = root.get<string>("scene.name");
	string sceneDate = root.get<string>("scene.date");

	return Scene(sceneName, sceneDate);
}

Scene FileController::createScene(string name)
{
	string scenePath = dataFolder + "/" + name;
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

bool FileController::hasCapture(Scene scene, Operation operation)
{
	string operationPath = dataFolder + "/" + scene.getName() + "/" + operation.toString();
	return filesystem::exists(operationPath + "/capture.json");
}

void FileController::saveCapture(Scene scene, Operation operation, Capture* capture)
{
	string operationPath = dataFolder + "/" + scene.getName() + "/" + operation.toString();
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

vector<int> FileController::getCapturedCameras(Scene scene, Operation operation)
{
	vector<int> capturedCameras;

	string capturePath = dataFolder + "/" + scene.getName() + "/" + operation.toString();
	property_tree::ptree root;
	property_tree::read_json(capturePath + "/capture.json", root);	

	for (property_tree::ptree::value_type &camera: root.get_child("capture.cameras"))
	{
		capturedCameras.push_back(camera.second.get_value<int>());
	}

	return capturedCameras;
}

bool FileController::hasCapturedFrame(Scene scene, Operation operation, int cameraNumber, int frameNumber)
{
	string frameFile = dataFolder + "/" + scene.getName() + "/" + operation.toString() + "/cam-" + to_string(cameraNumber) + "/" + to_string(frameNumber) + ".png";
	return filesystem::exists(frameFile);
}

Mat FileController::getCapturedFrame(Scene scene, Operation operation, int cameraNumber, int frameNumber)
{
	if (hasCapturedFrame(scene, operation, cameraNumber, frameNumber))
	{
		string frameFile = dataFolder + "/" + scene.getName() + "/" + operation.toString() + "/cam-" + to_string(cameraNumber) + "/" + to_string(frameNumber) + ".png";
		Mat frame = imread(frameFile);
		return frame;
	}
	else
	{
		return Mat();
	}	
}

Intrinsics* FileController::getIntrinsics(int cameraNumber) 
{
	string intrinsicsFile = dataFolder + "/intrinsics.json";
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

void FileController::saveIntrinsics(map<int, Intrinsics*> calibrationResults)
{
	string intrinsicsFile = dataFolder + "/intrinsics.json";
	property_tree::ptree root;

	time_t _tm = time(NULL);
	struct tm* curtime = localtime(&_tm);
	string date = asctime(curtime);
	root.put("calibration.date", date);

	property_tree::ptree camerasNode;
	for (pair<int, Intrinsics*> calibrationResult: calibrationResults)
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

void FileController::saveExtrinsics(Scene scene, map<int, Extrinsics*> extrinsicMatrices) 
{
	string extrinsicsFile = dataFolder + "/" + scene.getName() + "/extrinsics.json";
	property_tree::ptree root;

	time_t _tm = time(NULL);
	struct tm* curtime = localtime(&_tm);
	string date = asctime(curtime);
	root.put("calibration.date", date);

	property_tree::ptree camerasNode;
	for (pair<int, Extrinsics*> calibrationResult: extrinsicMatrices)
	{
		property_tree::ptree cameraNode;
		int cameraNumber = calibrationResult.first;
		Extrinsics* extrinsics = calibrationResult.second;
		
		cameraNode.put("cameraId", cameraNumber);

		Mat translationMatrix = extrinsics->getTranslationMatrix();
		cameraNode.put("translationMatrix.x", translationMatrix.at<double>(0, 0));
		cameraNode.put("translationMatrix.y", translationMatrix.at<double>(0, 1));
		cameraNode.put("translationMatrix.z", translationMatrix.at<double>(0, 2));

		Mat rotationMatrix = extrinsics->getRotationMatrix();
		cameraNode.put("rotationMatrix.x", rotationMatrix.at<double>(0, 0));
		cameraNode.put("rotationMatrix.y", rotationMatrix.at<double>(0, 1));
		cameraNode.put("rotationMatrix.z", rotationMatrix.at<double>(0, 2));

		camerasNode.push_back(make_pair("", cameraNode));
		delete extrinsics;
	}

	root.add_child("calibration.cameras", camerasNode);	
	property_tree::write_json(extrinsicsFile, root);
}

void FileController::saveCalibrationDetections(Mat detection, Scene scene, Operation operation, int cameraNumber, int frameNumber)
{
	string cameraFolder = dataFolder + "/" + scene.getName() + "/" + operation.toString() + "/cam-" + to_string(cameraNumber);

	if (filesystem::exists(cameraFolder))
	{
		string detectionsFolder = cameraFolder + "/detections";
		filesystem::create_directory(detectionsFolder);
		
		string detectionFile = detectionsFolder + "/" + to_string(frameNumber) + ".png";
		imwrite(detectionFile, detection);
	}
}

string FileController::getDataFolder()
{
	return dataFolder;
}

int FileController::getMaxWidth()
{
	return maxWidth;
}

int FileController::getMaxHeight()
{
	return maxHeight;
}

int FileController::getCameraHeight()
{
	return cameraHeight;
}

int FileController::getCameraWidth()
{
	return cameraWidth;
}

int FileController::getBarHeight()
{
	return barHeight;
}

int FileController::getGuiFps()
{
	return guiFps;
}

bool FileController::getShowPreviewOnCapture()
{
	return showPreviewOnCapture;
}

map<int, int> FileController::getCamerasOrder()
{
	return camerasOrder;
}

int FileController::getCamerasNumber()
{
	return camerasOrder.size();
}

int FileController::getCamerasFps()
{
	return camerasFps;
}

int FileController::getMaxCheckboards()
{
	return maxCheckboards;
}

int FileController::getCheckboardTimer()
{
	return checkboardTimer;
}

int FileController::getCheckboardCols()
{
	return checkboardCols;
}

int FileController::getCheckboardRows()
{
	return checkboardRows;
}

double FileController::getCheckboardSquareLength()
{
	return checkboardSquareLength;
}

double FileController::getCheckboardMarkerLength()
{
	return checkboardMarkerLength;
}

int FileController::getCheckboardWidth()
{
	return checkboardWidth;
}

int FileController::getCheckboardHeight()
{
	return checkboardHeight;
}

int FileController::getCheckboardMargin()
{
	return checkboardMargin;
}