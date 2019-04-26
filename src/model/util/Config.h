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
	int getMaxWidth();
	int getMaxHeight();
	int getCameraHeight();
	int getCameraWidth();
	int getBarHeight();
	int getGuiFps();
	int getMaxCheckboards();
	int getCheckboardInterval();
	int getCamerasNumber();
	map<int, int> getCamerasOrder();
	int getCamerasFps();
private:
	string dataPath;
	int maxWidth;
	int maxHeight;
	int cameraHeight;
	int cameraWidth;
	int barHeight;
	int guiFps;
	int maxCheckboards;
	int checkboardInterval;
	map<int, int> camerasOrder;
	int camerasFps;	
};
