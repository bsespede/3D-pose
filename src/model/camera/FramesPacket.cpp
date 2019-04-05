#include "FramesPacket.h"

FramesPacket::FramesPacket()
{
	this->frames = map<int, Mat>();
}

FramesPacket::FramesPacket(FramesPacket* orig)
{
	this->frames = map<int, Mat>(orig->getFrames());
}

void FramesPacket::addFrame(int camera, Mat frame)
{
	frames[camera] = frame;
}

map<int, Mat> FramesPacket::getFrames()
{
	return frames;
}