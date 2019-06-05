#pragma once

#include <map>
#include <list>
#include <opencv2/opencv.hpp>

class Frame3D
{
public:
	Frame3D();
	std::map<int, std::list<cv::Point3d>> getData();
	void addData(int index, std::list<cv::Point3d> pointcloud);
	bool hasData();
private:
	std::map<int, std::list<cv::Point3d>> data;
};
