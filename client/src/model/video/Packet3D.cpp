#include "Packet3D.h"

Packet3D::Packet3D()
{
	this->data = std::map<int, Frame3D*>();
}

std::map<int, Frame3D*> Packet3D::getData()
{
	return data;
}

void Packet3D::addData(int index, Frame3D* payload)
{
	data[index] = payload;
}

bool Packet3D::hasData()
{
	return data.empty();
}