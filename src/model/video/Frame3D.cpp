#include "Frame3D.h"

Frame3D::Frame3D()
{
	this->data = map<int, list<Point3d>>();
}

map<int, list<Point3d>> Frame3D::getData()
{
	return data;
}

void Frame3D::addData(int index, list<Point3d> payload)
{
	data[index] = payload;
}

bool Frame3D::hasData()
{
	return data.empty();
}