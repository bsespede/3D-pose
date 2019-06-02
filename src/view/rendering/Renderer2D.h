#pragma once

#include <opencv2/opencv.hpp>
#include "model/config/ConfigController.h"
#include "model/camera/capture/Packet.h"

using namespace cv;

class Renderer2D
{
public:
	Renderer2D(ConfigController* configController);
	void calculateProportions(float prop, int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int camerasNumber, int barHeight);
	void render(Packet* Packet);
private:
	int barHeight;
	int cameraWidth;
	int cameraHeight;
	int rows;
	int cols;	
};
