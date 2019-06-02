#pragma once

#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>
#include <atomic>
#include <thread>

#include "enum/Input.h"
#include "model/AppController.h"
#include "view/rendering/Renderer2D.h"

#define RED 0x0C
#define BLUE 0x09
#define GREEN 0x0A
#define WHITE 0x07

using namespace std;

class Console
{
public:
	Console(ConfigController* configController);
	void loopUI();
private:
	void showLogo();
	void showMenu();	
	void showSceneInput(Input input);
	void showOperations(Scene scene);
	void showOperationOptions(Scene scene, Operation operation);
	void showCapture(Scene scene, Operation operation);
	void showProcess(Scene scene, Operation operation);
	void showResults(Scene scene, Operation operation);
	void showPreview();
	void showPreviewLoop();
	void showStatusMessage(string message, int fontColor);
	ConfigController* configController;
	AppController* appController;
	Renderer2D* renderer2D;
	atomic<bool> showCamera;
};
