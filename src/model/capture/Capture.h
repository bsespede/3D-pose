#pragma once

#include <vector>
#include <list>
#include "model/camera/FramesPacket.h"

class Capture
{
public:
	Capture();
	void addToCaptureFrame(FramesPacket frame);
	void addToCaptureRecording(FramesPacket frame);
	std::list<FramesPacket> getRecording();
	std::vector<FramesPacket> getFrames();
private:
	std::vector<FramesPacket> frames;
	std::list<FramesPacket> recording;
};
