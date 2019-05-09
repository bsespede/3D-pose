#pragma once

#include <list>
#include "model/camera/FramesPacket.h"

using namespace std;

class Capture
{
public:
	Capture();
	void addFrame(FramesPacket* frame);
	std::list<FramesPacket*> getFrames();
private:
	std::list<FramesPacket*> frames;
};
