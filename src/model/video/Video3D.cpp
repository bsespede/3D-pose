#include "Video3D.h"

Video3D::Video3D(vector<int> cameras, map<int, Intrinsics*> intrinsics, map<int, Extrinsics*> extrinsics, map<int, Mat> frustumImages)
{
	this->currentFrame = 0;
	this->cameras = cameras;
	this->intrinsics = intrinsics;
	this->extrinsics = extrinsics;
	this->frustums = frustums;
	this->frames = vector<Frame3D*>();
}

vector<int> Video3D::getCameras()
{
	return cameras;
}

map<int, Intrinsics*> Video3D::getIntrinsics()
{
	return intrinsics;
}

map<int, Extrinsics*> Video3D::getExtrinsics()
{
	return extrinsics;
}

map<int, Mat> Video3D::getFrustums()
{
	return frustums;
}

Frame3D* Video3D::getNextFrame()
{
	currentFrame++;

	if (currentFrame == frames.size())
	{
		currentFrame = 0;
	}

	return frames[currentFrame];
}

void Video3D::addFrame(Frame3D* frame)
{
	frames.push_back(frame);
}