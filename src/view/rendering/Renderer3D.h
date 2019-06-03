#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include "model/capture/Result.h"
#include "model/config/ConfigController.h"

using namespace cv;
using namespace std;

class Renderer3D
{
public:
	Renderer3D(ConfigController* configController);
	void render(Result* result);
private:
	int totalSquares;
	int squareLength;
};
