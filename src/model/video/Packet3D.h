#pragma once

#include <map>
#include "model/video/Frame3D.h"

class Packet3D
{
public:
	Packet3D();
	std::map<int, Frame3D*> getData();
	void addData(int index, Frame3D* payload);
	bool hasData();
private:
	std::map<int, Frame3D*> data;
};
