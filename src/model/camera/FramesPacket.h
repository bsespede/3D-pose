#pragma once

#include <map>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class FramesPacket
{
public:
	FramesPacket();
	FramesPacket(FramesPacket* orig);
	void addFrame(int camera, Mat frame);
	map<int, Mat> getFrames();
	bool isEmpty();
private:
	map<int, Mat> frames;
};
