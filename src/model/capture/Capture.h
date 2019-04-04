#pragma once

#include <vector>
#include <list>
#include "model/camera/FramesPacket.h"

using namespace std;

class Capture
{
private:
	vector<FramesPacket> frames;
	list<FramesPacket> recording;
public:
	Capture();
	void addToCaptureFrame(FramesPacket frame);
	void addToCaptureRecording(FramesPacket frame);
	list<FramesPacket> getRecording();
	vector<FramesPacket> getFrames();
};
