#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

using namespace std;
using namespace boost;

class ConfigController
{
public:
	ConfigController();
	string getDataFolder();
	int getGuiFps();
	int getMaxWidth();
	int getMaxHeight();
	int getBarHeight();	
	bool getShowPreviewOnCapture();
	int getTotalSquares();
	int getSquareLength();
	int getCameraNumber();
	int getCameraLowFps();
	int getCameraHighFps();
	int getCameraHeight();
	int getCameraWidth();
	map<int, int> getCameraOrder();	
	int getMinCalibrationSamples();
	int getMaxCalibrationSamples();
	int getCharucoCols();
	int getCharucoRows();	
	float getCharucoSquareLength();
	float getCharucoMarkerLength();
private:
	string dataFolder;
	int guiFps;
	int maxWidth;
	int maxHeight;	
	int barHeight;	
	bool showPreviewOnCapture;
	int totalSquares;
	int squareLength;
	int cameraHighFps;
	int cameraLowFps;
	int cameraHeight;
	int cameraWidth;
	map<int, int> cameraOrder;	
	int minCalibrationSamples;
	int maxCalibrationSamples;
	int charucoCols;
	int charucoRows;
	float charucoSquareLength;
	float charucoMarkerLength;
};
