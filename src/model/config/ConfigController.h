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
	bool getShouldOutputDebugData();
	int getMaxSamplesIntrinsics();
	int getMaxSamplesExtrinsics();
	int getCharucoCols();
	int getCharucoRows();	
	float getCharucoSquareLength();
	float getCharucoMarkerLength();
	int getCameraFps();
	int getCameraNumber();
	std::map<int, std::pair<int, bool>> getCameraData();
private:
	std::string dataFolder;
	int guiFps;
	bool showPreviewOnCapture;
	bool shouldOutputDebugData;
	int maxSamplesIntrinsics;
	int maxSamplesExtrinsics;
	int charucoCols;
	int charucoRows;
	float charucoSquareLength;
	float charucoMarkerLength;
	int cameraFps;
	std::map<int, std::pair<int, bool>> cameraData;
};
