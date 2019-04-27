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

	int cameraNumber = 0;
	this->camerasOrder = std::map<int, int>();	
	this->camerasFps = root.get<int>("config.cameras.fps");
	for (property_tree::ptree::value_type &cameraId : root.get_child("config.cameras.camerasOrder"))
	{
		int serial = cameraId.second.get_value<int>();
		camerasOrder[serial] = cameraNumber;
		cameraNumber++;
	}
	
	this->maxCheckboards = root.get<int>("config.calibration.maxCheckboards");
	this->checkboardInterval = root.get<int>("config.calibration.checkboardInterval");

	this->checkboardName = root.get<string>("config.calibration.charucoCheckboard.name");
	this->checkboardCols = root.get<int>("config.calibration.charucoCheckboard.cols");
	this->checkboardRows = root.get<int>("config.calibration.charucoCheckboard.rows");
	this->checkboardSquareLength = root.get<float>("config.calibration.charucoCheckboard.squareLength");
	this->checkboardMarkerLength = root.get<float>("config.calibration.charucoCheckboard.markerLength");
	this->checkboardWidth = root.get<int>("config.calibration.charucoCheckboard.width");
	this->checkboardHeight = root.get<int>("config.calibration.charucoCheckboard.height");
	this->checkboardMargin = root.get<int>("config.calibration.charucoCheckboard.margin");
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

int Config::getCamerasNumber()
{
	return camerasOrder.size();
}

map<int, int> Config::getCamerasOrder()
{
	return camerasOrder;
}

int Config::getCamerasFps()
{
	return camerasFps;
}

int Config::getMaxCheckboards()
{
	return maxCheckboards;
}

int Config::getCheckboardInterval()
{
	return checkboardInterval;
}

string Config::getCheckboardName() 
{
	return checkboardName;
}

int Config::getCheckboardCols()
{
	return checkboardCols;
}

int Config::getCheckboardRows()
{
	return checkboardRows;
}

float Config::getCheckboardSquareLength()
{
	return checkboardSquareLength;
}

float Config::getCheckboardMarkerLength()
{
	return checkboardMarkerLength;
}

int Config::getCheckboardWidth()
{
	return checkboardWidth;
}

int Config::getCheckboardHeight()
{
	return checkboardHeight;
}

int Config::getCheckboardMargin()
{
	return checkboardMargin;
}