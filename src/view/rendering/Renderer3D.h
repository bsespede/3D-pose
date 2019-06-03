#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include "model/calibration/Intrinsics.h"
#include "model/calibration/Extrinsics.h"

using namespace cv;
using namespace std;

class Renderer3D
{
public:
	Renderer3D(int totalSquares, int squaresLength);
	void render(vector<int> cameras, map<int, Intrinsics*> intrinsics, map<int, Extrinsics*> extrinsics, map<int, Mat> frustumImage);
private:
	int totalSquares;
	int squareLength;
};
