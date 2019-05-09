#include "Capture.h"

Capture::Capture()
{
	this->frames = list<FramesPacket*>();
}

void Capture::addFrame(FramesPacket* frame)
{
	frames.push_back(frame);
}

list<FramesPacket*> Capture::getFrames()
{
	return frames;
}