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
	Console(Config* config, AppController* appController);
	void start();
	void showMenu();	
	void showSceneInput(Input input);
	void showOperations(Scene scene);
	void showOperationOptions(Scene scene, Operation operation);
	void showCapture(Scene scene, Operation operation);
	void showProcess(Scene scene, Operation operation);
	void showPreview();
	void showPreviewGUI();
	void showStatusMessage(string message, int fontColor);
private:
	AppController* appController;
	CameraRenderer* cameraRenderer;
	atomic<bool> showCamera;
	bool showPreviewOnCapture;
	int checkboardTimer;
	int guiFps;
};
