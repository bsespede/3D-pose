#pragma once

#include "scene/SceneController.h"
#include "camera/CameraController.h"
#include <string>

using namespace std;

class AppController
{
private:
	void loggerInit();
	string dataPath;
public:
	AppController(string dataPath);
	
};
