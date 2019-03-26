#include "Scene.h"

Scene::Scene(std::string name, SceneStatus status)
{
	this->name = name;
	this->status = status;
}

std::string Scene::getName() const
{
	return name;
}

SceneStatus Scene::getStatus() const
{
	return status;
}