#pragma once

#include <list>
#include <opencv2/opencv.hpp>

class Frame3D
{
public:
	Frame3D();
	std::list<cv::Point3d> getPointData();
	std::list<std::pair<cv::Point3d, cv::Point3d>> getLineData();
	void addData(cv::Point3d point);
	void addData(std::pair<cv::Point3d, cv::Point3d> line);
private:
	std::list<cv::Point3d> pointData;
	std::list<std::pair<cv::Point3d, cv::Point3d>> lineData;
};
