#include "SceneController.h"

SceneController::SceneController(string path)
{
	this->path = path;
}

bool SceneController::sceneExists(string name)
{
	return filesystem::exists(path + "/" + name + "/config.json");
}

Scene SceneController::loadScene(string name)
{
	string scenePath = path + "/" + name;

	property_tree::ptree root;
	property_tree::read_json(scenePath + "/config.json", root);
	
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

	string configFile = scenePath + "/config.json";
	property_tree::ptree root;

	root.put("scene.name", name);
	root.put("scene.date", date);

	property_tree::write_json(configFile, root);
	return Scene(name, date);
}

void SceneController::saveCapture(Scene scene, Operation operation, Capture* capture)
{
	string operationPath = path + "/" + scene.getName() + "/" + operation.toString();
	filesystem::create_directory(operationPath);

	if (operation == Operation::INTRINSICS)
	{
		vector<FramesPacket*> frames = capture->getFrames();

		for (int checkboardNumber = 0; checkboardNumber < frames.size(); checkboardNumber++)
		{
			for (pair<int, Mat> pair : frames[checkboardNumber]->getFrames())
			{
				string camPath = operationPath + "/cam-" + to_string(pair.first);
				filesystem::create_directory(camPath);

				string framePath = camPath + "/" + to_string(checkboardNumber) + ".png";
				imwrite(framePath, pair.second);
			}
		}
	}
	else if (operation == Operation::EXTRINSICS)
	{
		vector<FramesPacket*> frames = capture->getFrames();
		vector<string> labels = vector<string>{"empty", "axis"};

		for (int labelNumber = 0; labelNumber < frames.size(); labelNumber++)
		{
			for (pair<int, Mat> pair : frames[labelNumber]->getFrames())
			{
				string camPath = operationPath + "/cam-" + to_string(pair.first);
				filesystem::create_directory(camPath);

				string framePath = camPath + "/" + labels[labelNumber] + ".png";
				imwrite(framePath, pair.second);
			}
		}

		list<FramesPacket*> recording = capture->getRecording();
		int frameNumber = 0;

		for (FramesPacket* framePacket: recording)
		{
			for (pair<int, Mat> pair : framePacket->getFrames())
			{
				string camPath = operationPath + "/cam-" + to_string(pair.first);
				filesystem::create_directory(camPath);

				string framePath = camPath + "/" + to_string(frameNumber) + ".png";
				imwrite(framePath, pair.second);
			}

			frameNumber++;
		}
	}
	else
	{
		list<FramesPacket*> recording = capture->getRecording();
		int frameNumber = 0;

		for (FramesPacket* framePacket : recording)
		{
			for (pair<int, Mat> pair : framePacket->getFrames())
			{
				string camPath = operationPath + "/cam-" + to_string(pair.first);
				filesystem::create_directory(camPath);

				string framePath = camPath + "/" + to_string(frameNumber) + ".png";
				imwrite(framePath, pair.second);
			}

			frameNumber++;
		}
	}

	delete capture;
}

vector<string> SceneController::getCaptureFolders(Scene scene, Operation operation)
{
	string scenePath = path + "/" + scene.getName() + "/" + operation.toString();
	vector<string> directories;

	for (filesystem::directory_entry& entry: filesystem::directory_iterator(scenePath))
	{
		directories.push_back(scenePath + "/" + entry.path().leaf().string());
	}

	return directories;
}

void SceneController::dumpIntrinsics(vector<IntrinsicCalibration> intrinsicMatrices)
{
	time_t _tm = time(NULL);
	struct tm* curtime = localtime(&_tm);
	string date = asctime(curtime);

	string configFile = path + "/intrinsic_calibration.json";
	property_tree::ptree root;

	for (int cameraIndex = 0; cameraIndex < intrinsicMatrices.size(); cameraIndex++)
	{
		root.put("calibration.cam-" + to_string(cameraIndex) + ".reprojectionError", intrinsicMatrices[cameraIndex].getReprojectionError());

		Mat cameraMatrix = intrinsicMatrices[cameraIndex].getCameraMatrix();
		root.put("calibration.cam-" + to_string(cameraIndex) + ".calibrationMatrix.fx", cameraMatrix.at<double>(0, 0));
		root.put("calibration.cam-" + to_string(cameraIndex) + ".calibrationMatrix.fy", cameraMatrix.at<double>(1, 1));
		root.put("calibration.cam-" + to_string(cameraIndex) + ".calibrationMatrix.cx", cameraMatrix.at<double>(0, 2));
		root.put("calibration.cam-" + to_string(cameraIndex) + ".calibrationMatrix.cy", cameraMatrix.at<double>(1, 2));

		Mat distortionCoeffs = intrinsicMatrices[cameraIndex].getDistortionCoeffs();
		root.put("calibration.cam-" + to_string(cameraIndex) + ".distortionCoefficients.k1", distortionCoeffs.at<double>(0, 0));
		root.put("calibration.cam-" + to_string(cameraIndex) + ".distortionCoefficients.k2", distortionCoeffs.at<double>(0, 1));
		root.put("calibration.cam-" + to_string(cameraIndex) + ".distortionCoefficients.p1", distortionCoeffs.at<double>(0, 2));
		root.put("calibration.cam-" + to_string(cameraIndex) + ".distortionCoefficients.p2", distortionCoeffs.at<double>(0, 3));
		root.put("calibration.cam-" + to_string(cameraIndex) + ".distortionCoefficients.k3", distortionCoeffs.at<double>(0, 4));
	}
	
	property_tree::write_json(configFile, root);
}