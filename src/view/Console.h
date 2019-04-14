#pragma once

#include <windows.h>
#include <conio.h>
#include <iostream>
#include <atomic>
#include <thread>

#include "enum/Input.h"
#include "controller/AppController.h"
#include "view/CameraRenderer.h"

#define RED 0x0C
#define BLUE 0x09
#define GREEN 0x0A
#define WHITE 0x07

using namespace std;

class Console
{
public:
	Console(AppController* appController);
	void start();
	void showMenu();
	void showInputName(Input input);
	void showOperations(Scene scene);
	void showOperationOptions(Scene scene, Operation operation);
	void showOverwrite(Scene scene, Operation operation);
	void showCapture(Scene scene, Operation operation);
	void showCameras();
	void showProcess(Scene scene, Operation operation);
	void showCamerasTest();
	void showStatusMessage(string message, int fontColor);
private:
	AppController* appController;
	CameraRenderer* cameraRenderer;
	atomic<bool> showCamera;
	int guiFps;
};
