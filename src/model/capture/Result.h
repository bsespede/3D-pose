#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include "model/calibration/Intrinsics.h"
#include "model/calibration/Extrinsics.h"

using namespace std;

class Result
{
public:
	Result(vector<int> cameras, map<int, Intrinsics*> intrinsics, map<int, Extrinsics*> extrinsics, map<int, Mat> frustumImages);
	vector<int> getCameras();
	map<int, Intrinsics*> getIntrinsics();
	map<int, Extrinsics*> getExtrinsics();
	map<int, Mat> getFrustumImages();
private:
	vector<int> cameras;
	map<int, Intrinsics*> intrinsics;
	map<int, Extrinsics*> extrinsics;
	map<int, Mat> frustumImages;
};