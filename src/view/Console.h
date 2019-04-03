#pragma once

#include <windows.h>
#include <conio.h>
#include <string>
#include "controller/AppController.h"

#define RED 0x0C
#define BLUE 0x09
#define GREEN 0x0A
#define WHITE 0x07

class Console
{
private:
	AppController& sceneManager;
public:
	Console(AppController& sceneManager);
	void start();
	void showMenuOptions();
	void showSceneCreation();
	void showSceneLoad();
	void showSceneOperations(std::string name);
	void showRecordScene(std::string name);
	void showCalibrationOptions(std::string name);
	void showCalibrateScene(std::string name, CalibrationMode calibrationMode);
	void showStatusMessage(std::string message, int fontColor);
};
