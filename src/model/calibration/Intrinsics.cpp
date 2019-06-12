#include "Intrinsics.h"

Intrinsics::Intrinsics(cv::Mat cameraMatrix, cv::Mat distortionCoefficients, double reprojectionError)
{
	this->cameraMatrix = cameraMatrix;
	this->distortionCoefficients = distortionCoefficients;
	this->reprojectionError = reprojectionError;
}

cv::Mat Intrinsics::getCameraMatrix()
{
	return cameraMatrix;
}

cv::Mat Intrinsics::getDistortionCoefficients()
{
	return distortionCoefficients;
}

double Intrinsics::getReprojectionError()
{
	return reprojectionError;
}
