#include "Scene.h"

Scene::Scene(string name, string date)
{
	this->name = name;
	this->date = date;
}

string Scene::getDate() const
{
	return date;
}

string Scene::getName() const
{
	return name;
}