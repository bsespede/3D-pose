#pragma once

#include <vector>
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
	vector<int> getCamerasOrder();
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
	vector<int> camerasOrder;
	int camerasFps;	
};
