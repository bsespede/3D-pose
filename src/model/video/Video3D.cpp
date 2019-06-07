#include "Video3D.h"

Video3D::Video3D(std::vector<int> cameras, std::map<int, Intrinsics*> intrinsics, std::map<int, Extrinsics*> extrinsics)
{
	this->frameNumber = 0;
	this->cameras = cameras;
	this->intrinsics = intrinsics;
	this->extrinsics = extrinsics;
	this->reconstruction = std::vector<Packet3D*>();
	this->playing = true;
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

Packet3D* Video3D::getNextPacket()
{
	frameNumber++;

	if (frameNumber == reconstruction.size())
	{
		frameNumber = 0;
	}

	return reconstruction[frameNumber];
}

void Video3D::togglePlayback()
{
	playing = !playing;
}

bool Video3D::isPlaying()
{
	return playing;
}

void Video3D::addPacket(Packet3D* packet)
{
	reconstruction.push_back(packet);
}