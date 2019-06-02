#include "Intrinsics.h"

Intrinsics::Intrinsics(Mat cameraMatrix, Mat distortionCoefficients, double reprojectionError)
{
	this->cameraMatrix = cameraMatrix;
	this->distortionCoefficients = distortionCoefficients;
	this->reprojectionError = reprojectionError;
}

Mat Intrinsics::getCameraMatrix()
{
	return cameraMatrix;
}

Mat Intrinsics::getDistortionCoefficients()
{
	return distortionCoefficients;
}

double Intrinsics::getReprojectionError()
{
	return reprojectionError;
}
