#include "SceneController.h"

SceneController::SceneController(string path): path(path)
{
	
}

bool SceneController::sceneExists(string name)
{
	return filesystem::exists(path + "/" + name + "/config.json");
}

Scene SceneController::loadScene(string name)
{
	string configFile = path + "/" + name + "/config.json";
	property_tree::ptree root;
	property_tree::read_json(configFile, root);

	string sceneName = root.get<string>("scene.name");
	string sceneDate = root.get<string>("scene.date");

	return Scene(sceneName, sceneDate);
}

Scene SceneController::createScene(string name)
{
	time_t _tm = time(NULL);
	struct tm * curtime = localtime(&_tm);
	string date = asctime(curtime);

	string configFile = path + "/" + name + "/config.json";
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