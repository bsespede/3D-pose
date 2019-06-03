#include "Result.h"

Result::Result(vector<int> cameras, map<int, Intrinsics*> intrinsics, map<int, Extrinsics*> extrinsics, map<int, Mat> frustumImages)
{
	this->cameras = cameras;
	this->intrinsics = intrinsics;
	this->extrinsics = extrinsics;
	this->frustumImages = frustumImages;
}

vector<int> Result::getCameras()
{
	return cameras;
}

map<int, Intrinsics*> Result::getIntrinsics()
{
	return intrinsics;
}

map<int, Extrinsics*> Result::getExtrinsics()
{
	return extrinsics;
}

map<int, Mat> Result::getFrustumImages()
{
	return frustumImages;
}