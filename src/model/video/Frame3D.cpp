#include "Frame3D.h"

Frame3D::Frame3D()
{
	this->pointData = std::list<cv::Point3d>();
	this->lineData = std::list<std::pair<cv::Point3d, cv::Point3d>>();
}

std::list<cv::Point3d> Frame3D::getPointData()
{
	return pointData;
}

std::list<std::pair<cv::Point3d, cv::Point3d>> Frame3D::getLineData()
{
	return lineData;
}

void Frame3D::addData(cv::Point3d point)
{
	pointData.push_back(point);
}

void Frame3D::addData(std::pair<cv::Point3d, cv::Point3d> line)
{
	lineData.push_back(line);
}