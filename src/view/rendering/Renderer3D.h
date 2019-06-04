#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include "model/video/Video3D.h"
#include "model/config/ConfigController.h"

using namespace cv;
using namespace std;

class Renderer3D
{
public:
	Renderer3D(ConfigController* configController);
	void render(Video3D* result);
private:
	int guiFps;
	int totalSquares;
	int squareLength;
};
