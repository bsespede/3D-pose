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

bool SceneController::hasCapture(Scene scene, Operation operation)
{
	return filesystem::exists(path + "/" + scene.getName() + "/" + operation.toString());
}

bool SceneController::hasProcessed(Scene scene, Operation operation)
{
	return filesystem::exists(path + "/" + scene.getName() + "/" + operation.toString() + ".json");
}

void SceneController::deleteCapture(Scene scene, Operation operation)
{
	filesystem::remove_all(path + "/" + scene.getName() + "/" + operation.toString());
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