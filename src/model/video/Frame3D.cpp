#include "Frame3D.h"

Frame3D::Frame3D()
{
	this->data = std::map<int, std::list<cv::Point3d>>();
}

std::map<int, std::list<cv::Point3d>> Frame3D::getData()
{
	return data;
}

void Frame3D::addData(int index, std::list<cv::Point3d> payload)
{
	data[index] = payload;
}

bool Frame3D::hasData()
{
	return data.empty();
}