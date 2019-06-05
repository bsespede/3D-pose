#include "Scene.h"

Scene::Scene(std::string name, std::string date)
{
	this->name = name;
	this->date = date;
}

std::string Scene::getDate()
{
	return date;
}

std::string Scene::getName()
{
	return name;
}