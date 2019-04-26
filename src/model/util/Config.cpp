#include "Config.h"

Config::Config() 
{
	property_tree::ptree root;
	property_tree::read_json("app-config.json", root);

	this->dataPath = root.get<string>("config.dataPath");

	this->maxWidth = root.get<int>("config.gui.maxWidth");
	this->maxHeight = root.get<int>("config.gui.maxHeight");
	this->cameraHeight = root.get<int>("config.gui.cameraHeight");
	this->cameraWidth = root.get<int>("config.gui.cameraWidth");
	this->barHeight = root.get<int>("config.gui.barHeight");
	this->guiFps = root.get<int>("config.gui.fps");

	this->camerasOrder = std::vector<int>();
	for (property_tree::ptree::value_type &cameraId : root.get_child("cameras.camerasOrder"))
	{
		camerasOrder.push_back(cameraId.second.get_value<int>());
	}

	this->maxCheckboards = root.get<int>("config.cameras.maxCheckboards");
	this->checkboardInterval = root.get<int>("config.cameras.checkboardInterval");
	this->camerasFps = root.get<int>("config.cameras.fps");	
}

string Config::getDataPath()
{
	return dataPath;
}

int Config::getMaxWidth()
{
	return maxWidth;
}

int Config::getMaxHeight()
{
	return maxHeight;
}

int Config::getCameraHeight()
{
	return cameraHeight;
}

int Config::getCameraWidth()
{
	return cameraWidth;
}

int Config::getBarHeight()
{
	return barHeight;
}

int Config::getGuiFps()
{
	return guiFps;
}

int Config::getMaxCheckboards()
{
	return maxCheckboards;
}

int Config::getCheckboardInterval()
{
	return checkboardInterval;
}

int Config::getCamerasNumber()
{
	return camerasOrder.size();
}

vector<int> Config::getCamerasOrder()
{
	return camerasOrder;
}

int Config::getCamerasFps()
{
	return camerasFps;
}