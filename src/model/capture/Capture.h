#pragma once

#include <vector>
#include <list>
#include <string>
#include "model/camera/FramesPacket.h"

using namespace std;

class Capture
{
public:
	Capture();
	void addToCaptureRecording(FramesPacket* frame);
	std::list<FramesPacket*> getRecording();
	void addToCaptureFrame(FramesPacket* frame);	
	std::vector<FramesPacket*> getFrames();
private:
	std::vector<FramesPacket*> frames;
	std::list<FramesPacket*> recording;
};
