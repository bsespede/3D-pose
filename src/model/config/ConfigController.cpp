#include "ConfigController.h"

ConfigController::ConfigController()
{
	property_tree::ptree root;
	property_tree::read_json("app-config.json", root);

	this->dataFolder = root.get<string>("config.dataFolder");

	this->guiFps = root.get<int>("config.gui.fps");
	this->maxWidth = root.get<int>("config.gui.maxWidth");
	this->maxHeight = root.get<int>("config.gui.maxHeight");	
	this->barHeight = root.get<int>("config.gui.barHeight");	
	this->showPreviewOnCapture = root.get<bool>("config.gui.showCapturePreview");
	
	this->cameraLowFps = root.get<int>("config.cameras.lowFps");
	this->cameraHighFps = root.get<int>("config.cameras.highFps");
	this->cameraHeight = root.get<int>("config.cameras.cameraHeight");
	this->cameraWidth = root.get<int>("config.cameras.cameraWidth");

	int cameraNumber = 0;
	this->cameraOrder = std::map<int, int>();
	for (property_tree::ptree::value_type& cameraId : root.get_child("config.cameras.order"))
	{
		int serial = cameraId.second.get_value<int>();
		cameraOrder[serial] = cameraNumber;
		cameraNumber++;
	}
	
	this->minCalibrationSamples = root.get<int>("config.calibration.minSamples");
	this->maxCalibrationSamples = root.get<int>("config.calibration.maxSamples");

	this->charucoCols = root.get<int>("config.calibration.charucoBoard.cols");
	this->charucoRows = root.get<int>("config.calibration.charucoBoard.rows");
	this->charucoSquareLength = root.get<float>("config.calibration.charucoBoard.squareLength");
	this->charucoMarkerLength = root.get<float>("config.calibration.charucoBoard.markerLength");
}

string ConfigController::getDataFolder()
{
	return dataFolder;
}

int ConfigController::getGuiFps()
{
	return guiFps;
}

int ConfigController::getMaxWidth()
{
	return maxWidth;
}

int ConfigController::getMaxHeight()
{
	return maxHeight;
}

int ConfigController::getCameraHeight()
{
	return cameraHeight;
}

int ConfigController::getCameraWidth()
{
	return cameraWidth;
}

int ConfigController::getBarHeight()
{
	return barHeight;
}

bool ConfigController::getShowPreviewOnCapture()
{
	return showPreviewOnCapture;
}

map<int, int> ConfigController::getCameraOrder()
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

int ConfigController::getMinCalibrationSamples()
{
	return minCalibrationSamples;
}

int ConfigController::getMaxCalibrationSamples()
{
	return maxCalibrationSamples;
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