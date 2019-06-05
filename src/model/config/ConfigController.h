#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

class ConfigController
{
public:
	ConfigController();
	std::string getDataFolder();
	int getGuiFps();
	bool getShowPreviewOnCapture();
	int getCameraNumber();
	int getCameraLowFps();
	int getCameraHighFps();
	std::map<int, int> getCameraOrder();
	bool getOutputDetections();
	int getCharucoCols();
	int getCharucoRows();	
	float getCharucoSquareLength();
	float getCharucoMarkerLength();
private:
	std::string dataFolder;
	int guiFps;
	bool showPreviewOnCapture;
	int cameraHighFps;
	int cameraLowFps;
	std::map<int, int> cameraOrder;
	bool outputDetections;
	int charucoCols;
	int charucoRows;
	float charucoSquareLength;
	float charucoMarkerLength;
};
