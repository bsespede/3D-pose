#include "Scene.h"

Scene::Scene(string name, string date) : name(name), date(date)
{
}

string Scene::getDate()
{
	return date;
}

string Scene::getName()
{
	return name;
}