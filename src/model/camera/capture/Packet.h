#pragma once

#include <map>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Packet
{
public:
	Packet();
	Packet(Packet* packet);
	map<int, Mat> getData();
	void addData(int index, Mat payload);
	bool hasData();
private:
	map<int, Mat> data;
};
