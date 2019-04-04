#pragma once

#include <map>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class FramesPacket
{
public:
	FramesPacket();
	void addFrame(int camera, Mat frame);
	map<int, Mat> getFrames();
private:
	map<int, Mat> frames;
};
