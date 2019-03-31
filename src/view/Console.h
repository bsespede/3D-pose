#pragma once

#include <windows.h>
#include <conio.h>
#include <string>
#include <chrono>
#include <thread>
#include "model/scene/SceneManager.h"
#include "model/scene/enum/CaptureMode.h"
#include "model/scene/enum/CalibrationMode.h"

#define RED 0x0C
#define BLUE 0x09
#define GREEN 0x0A
#define WHITE 0x07

class Console
{
private:
	SceneManager& sceneManager;
public:
	Console(SceneManager& sceneManager);
	void start();
	void showMenuOptions();
	void showSceneCreation();
	void showSceneLoad();
	void showSceneOperations(std::string name);
	void showRecordScene(std::string name, CaptureMode captureMode);
	void showCalibrateScene(std::string name);
	void showProcessScene(std::string name);
	void showStatusMessage(std::string message, int fontColor);
};
