#pragma once

#include <opencv2/opencv.hpp>
#include "model/camera/FramesPacket.h"
#include "controller/files/FileController.h"

using namespace cv;

class CameraRenderer
{
public:
	CameraRenderer(FileController* fileController);
	void calculateSizeProportion(float prop, int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int camerasNumber, int barHeight);
	void render(FramesPacket* framesPacket);
private:
	int barHeight;
	int cameraWidth;
	int cameraHeight;
	int rows;
	int cols;
	float prop;
};
