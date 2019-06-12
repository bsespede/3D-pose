#include "ConfigController.h"

ConfigController::ConfigController()
{
	boost::property_tree::ptree root;
	boost::property_tree::read_json("app-config.json", root);

	this->dataFolder = root.get<std::string>("config.dataFolder");
	this->guiFps = root.get<int>("config.gui.fps");
	this->showPreviewOnCapture = root.get<bool>("config.gui.showCapturePreview");	

	this->shouldOutputDebugData = root.get<bool>("config.calibration.shouldOutputDebugData");
	this->maxSamplesIntrinsics = root.get<int>("config.calibration.maxSamplesIntrinsics");
	this->maxSamplesExtrinsics = root.get<int>("config.calibration.maxSamplesExtrinsics");
	this->charucoCols = root.get<int>("config.calibration.charucoCols");
	this->charucoRows = root.get<int>("config.calibration.charucoRows");
	this->charucoSquareLength = root.get<float>("config.calibration.charucoSquareLength");
	this->charucoMarkerLength = root.get<float>("config.calibration.charucoMarkerLength");

	this->cameraFps = root.get<int>("config.camera.fps");
	this->cameraData = std::map<int, std::pair<int, bool>>();
	for (boost::property_tree::ptree::value_type& cameraNode : root.get_child("config.camera.list"))
	{
		int cameraId = cameraNode.second.get<int>("id");
		int cameraSerial = cameraNode.second.get<int>("serial");
		bool cameraCapture = cameraNode.second.get<bool>("capture");

		std::pair<int, bool> cameraInfo = std::pair<int, bool>(cameraId, cameraCapture);
		cameraData[cameraSerial] = cameraInfo;
	}
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

bool ConfigController::getShouldOutputDebugData()
{
	return shouldOutputDebugData;
}

int ConfigController::getMaxSamplesIntrinsics()
{
	return maxSamplesIntrinsics;
}

int ConfigController::getMaxSamplesExtrinsics()
{
	return maxSamplesExtrinsics;
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

int ConfigController::getCameraFps()
{
	return cameraFps;
}

int ConfigController::getCameraNumber()
{
	return (int)cameraData.size();
}

std::map<int, std::pair<int, bool>> ConfigController::getCameraData()
{
	return cameraData;
}