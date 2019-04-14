#include "Scene.h"

Scene::Scene(string name, string date)
{
	this->name = name;
	this->date = date;
}

string Scene::getDate()
{
	return date;
}

string Scene::getName()
{
	return name;
}