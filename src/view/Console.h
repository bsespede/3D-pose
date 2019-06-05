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
#include "view/rendering/Renderer3D.h"

#define RED 0x0C
#define BLUE 0x09
#define GREEN 0x0A
#define WHITE 0x07

class Console
{
public:
	Console(ConfigController* configController);
	void start();
private:
	void showLogo();
	void showMenu();	
	void showSceneInput(Input input);
	void showSceneOptions(Scene scene);
	void showCalibrationOptions(Scene scene);
	void showMocapOptions(Scene scene);
	void showCapture(Scene scene, CaptureType captureType);
	void showProcessCalibration(Scene scene, CalibrationType calibrationType);
	void showProcessMocap(Scene scene);
	void showResultPreview(Scene scene, CaptureType captureType);
	void showCameraPreview();
	void showCameraPreviewLoop();
	void showStatusMessage(std::string message, int fontColor);
	bool showPreviewOnCapture;
	std::atomic<bool> showPreviewUI;
	AppController* appController;
	Renderer2D* renderer2D;
	Renderer3D* renderer3D;
};
