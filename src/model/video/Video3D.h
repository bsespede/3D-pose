#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include "model/calibration/Intrinsics.h"
#include "model/calibration/Extrinsics.h"
#include "model/video/Frame3D.h"

using namespace std;

class Video3D
{
public:
	Video3D(vector<int> cameras, map<int, Intrinsics*> intrinsics, map<int, Extrinsics*> extrinsics, map<int, Mat> frustums);
	vector<int> getCameras();
	map<int, Intrinsics*> getIntrinsics();
	map<int, Extrinsics*> getExtrinsics();
	map<int, Mat> getFrustums();
	Frame3D* getNextFrame();
	void addFrame(Frame3D* frame);	
private:
	int currentFrame;
	vector<int> cameras;
	map<int, Intrinsics*> intrinsics;
	map<int, Extrinsics*> extrinsics;
	map<int, Mat> frustums;	
	vector<Frame3D*> frames;
};