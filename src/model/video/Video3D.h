#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include "model/calibration/Intrinsics.h"
#include "model/calibration/Extrinsics.h"
#include "model/video/Frame3D.h"

class Video3D
{
public:
	Video3D(std::vector<int> cameras, std::map<int, Intrinsics*> intrinsics, std::map<int, Extrinsics*> extrinsics, std::map<int, cv::Mat> frustums);
	std::vector<int> getCameras();
	std::map<int, Intrinsics*> getIntrinsics();
	std::map<int, Extrinsics*> getExtrinsics();
	std::map<int, cv::Mat> getFrustums();
	Frame3D* getNextFrame();
	void addFrame(Frame3D* frame);	
private:
	int currentFrame;
	std::vector<int> cameras;
	std::map<int, Intrinsics*> intrinsics;
	std::map<int, Extrinsics*> extrinsics;
	std::map<int, cv::Mat> frustums;
	std::vector<Frame3D*> frames;
};