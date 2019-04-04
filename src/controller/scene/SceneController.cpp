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
	// TODO: Load json
	string name = ...;
	string date = ...;
	return Scene(name, date);
}

Scene SceneController::createScene(string name)
{
	string date = ...;
	// TODO: Create json
	return Scene(name, date);
}

bool SceneController::hasCapture(Scene scene, Operation operation)
{
	return filesystem::exists(path + "/" + name + "/" + operation.toString());
}

bool SceneController::hasProcessed(Scene scene, Operation operation)
{
	return filesystem::exists(path + "/" + name + "/" + operation.toString() + ".json");
}

void SceneController::deleteCapture(Scene scene, Operation operation)
{
	filesystem::remove_all(path + "/" + name + "/" + operation.toString());
}