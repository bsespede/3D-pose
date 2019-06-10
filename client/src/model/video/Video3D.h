#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include "model/calibration/Intrinsics.h"
#include "model/calibration/Extrinsics.h"
#include "model/video/Packet3D.h"

class Video3D
{
public:
	Video3D(std::vector<int> cameras, std::map<int, Intrinsics*> intrinsics, std::map<int, Extrinsics*> extrinsics);
	std::vector<int> getCameras();
	std::map<int, Intrinsics*> getIntrinsics();
	std::map<int, Extrinsics*> getExtrinsics();
	Packet3D* getNextPacket();
	void addPacket(Packet3D* frame);
	int getFrameNumber();	
	void togglePlayback();
	bool isPlaying();
private:
	int frameNumber;
	bool playing;	
	std::vector<int> cameras;
	std::map<int, Intrinsics*> intrinsics;
	std::map<int, Extrinsics*> extrinsics;
	std::vector<Packet3D*> reconstruction;
};