#include "Video3D.h"

Video3D::Video3D(std::vector<int> cameras, std::map<int, Intrinsics*> intrinsics, std::map<int, Extrinsics*> extrinsics)
{
	this->frameNumber = 0;
	this->cameras = cameras;
	this->intrinsics = intrinsics;
	this->extrinsics = extrinsics;
	this->frames = std::vector<Frame3D*>();
}

std::vector<int> Video3D::getCameras()
{
	return cameras;
}

std::map<int, Intrinsics*> Video3D::getIntrinsics()
{
	return intrinsics;
}

std::map<int, Extrinsics*> Video3D::getExtrinsics()
{
	return extrinsics;
}

int Video3D::getFrameNumber()
{
	return frameNumber;
}

Frame3D* Video3D::getNextFrame()
{
	frameNumber++;

	if (frameNumber == frames.size())
	{
		frameNumber = 0;
	}

	return frames[frameNumber];
}

void Video3D::addFrame(Frame3D* frame)
{
	frames.push_back(frame);
}