#pragma once

#include <map>
#include <list>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Frame3D
{
public:
	Frame3D();
	map<int, list<Point3d>> getData();
	void addData(int index, list<Point3d> pointcloud);
	bool hasData();
private:
	map<int, list<Point3d>> data;
};
