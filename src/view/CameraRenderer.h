#pragma once

#include <opencv2/opencv.hpp>
#include "model/camera/FramesPacket.h"

using namespace cv;

class CameraRenderer
{
public:
	CameraRenderer(int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int cameraNumber, int barHeight);
	void calculateSizeProportion(float prop, int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int cameraNumber, int barHeight);
	void render(FramesPacket* framesPacket);
private:
	int barHeight;
	int cameraWidth;
	int cameraHeight;
	int rows;
	int cols;
	float prop;
};
