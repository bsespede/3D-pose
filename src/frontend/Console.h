#pragma once

#include <windows.h>
#include <cstdio>
#include <conio.h>
#include <iostream>
#include <string>
#include "backend/scene/SceneManager.h"

#define RED 0x0C
#define BLUE 0x09
#define GREEN 0x0A
#define WHITE 0x07

class Console
{
private:
	SceneManager sceneManager;
public:
	Console(SceneManager sceneManager);
	void main();
	void showMenuOptions();
	void showSceneCreation();
	void showSceneLoad();
	void showSceneOperations(Scene scene);
	void showRecordScene(Scene scene, CaptureMode captureMode);
	void showCalibrateScene(Scene scene);
	void showProcessScene(Scene scene);
	void showStatusMessage(std::string message, int fontColor);
};
