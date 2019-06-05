#include "ConfigController.h"

ConfigController::ConfigController()
{
	boost::property_tree::ptree root;
	boost::property_tree::read_json("app-config.json", root);

	this->dataFolder = root.get<std::string>("config.dataFolder");
	this->guiFps = root.get<int>("config.gui.fps");
	this->showPreviewOnCapture = root.get<bool>("config.gui.showCapturePreview");	
	this->cameraLowFps = root.get<int>("config.cameras.lowFps");
	this->cameraHighFps = root.get<int>("config.cameras.highFps");
	int cameraNumber = 0;

	this->cameraOrder = std::map<int, int>();
	for (boost::property_tree::ptree::value_type& cameraId : root.get_child("config.cameras.order"))
	{
		int serial = cameraId.second.get_value<int>();
		cameraOrder[serial] = cameraNumber;
		cameraNumber++;
	}

	this->charucoCols = root.get<int>("config.calibration.charucoCols");
	this->charucoRows = root.get<int>("config.calibration.charucoRows");
	this->charucoSquareLength = root.get<float>("config.calibration.charucoSquareLength");
	this->charucoMarkerLength = root.get<float>("config.calibration.charucoMarkerLength");
}

std::string ConfigController::getDataFolder()
{
	return dataFolder;
}

int ConfigController::getGuiFps()
{
	return guiFps;
}

bool ConfigController::getShowPreviewOnCapture()
{
	return showPreviewOnCapture;
}

std::map<int, int> ConfigController::getCameraOrder()
{
	return cameraOrder;
}

int ConfigController::getCameraHighFps()
{
	return cameraHighFps;
}

int ConfigController::getCameraLowFps()
{
	return cameraLowFps;
}

int ConfigController::getCameraNumber()
{
	return (int)cameraOrder.size();
}

int ConfigController::getCharucoCols()
{
	return charucoCols;
}

int ConfigController::getCharucoRows()
{
	return charucoRows;
}

float ConfigController::getCharucoSquareLength()
{
	return charucoSquareLength;
}

float ConfigController::getCharucoMarkerLength()
{
	return charucoMarkerLength;
}