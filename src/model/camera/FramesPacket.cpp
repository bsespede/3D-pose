#include "FramesPacket.h

FramesPacket::FramesPacket()
{
	frames = map<int, Mat>();
}

void FramesPacket::addFrame(int camera, Mat frame)
{
	frames[camera] = frame;
}

map<int, Mat> FramesPacket::getFrames()
{
	return frames;
}