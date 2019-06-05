#pragma once

#include <map>
#include <opencv2/opencv.hpp>

class Packet
{
public:
	Packet();
	Packet(Packet* packet);
	std::map<int, cv::Mat> getData();
	void addData(int index, cv::Mat payload);
	bool hasData();
private:
	std::map<int, cv::Mat> data;
};
