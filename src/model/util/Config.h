#pragma once

#include <map>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost;
using namespace std;

class Config
{
public:
	Config();
	string getDataPath();

	// GUI
	int getMaxWidth();
	int getMaxHeight();
	int getCameraHeight();
	int getCameraWidth();
	int getBarHeight();
	int getGuiFps();
	bool getShowPreviewOnCapture();

	// Cameras
	map<int, int> getCamerasOrder();
	int getCamerasNumber();
	int getCamerasFps();

	// Calibration
	int getMaxCheckboards();
	int getCheckboardTimer();
	
	// CharUco
	string getCheckboardName();
	int getCheckboardCols();
	int getCheckboardRows();
	float getCheckboardSquareLength();
	float getCheckboardMarkerLength();
	int getCheckboardWidth();
	int getCheckboardHeight();
	int getCheckboardMargin();
private:
	string dataPath;

	// GUI
	int maxWidth;
	int maxHeight;
	int cameraHeight;
	int cameraWidth;
	int barHeight;
	int guiFps;
	bool showPreviewOnCapture;

	// Cameras
	map<int, int> camerasOrder;
	int camerasFps;

	// CharUco
	int maxCheckboards;
	int checkboardTimer;

	// CharUco
	string checkboardName;	
	int checkboardCols;
	int checkboardRows;
	float checkboardSquareLength;
	float checkboardMarkerLength;
	int checkboardWidth;
	int checkboardHeight;
	int checkboardMargin;
};
