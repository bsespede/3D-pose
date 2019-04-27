#include "IntrinsicCalibration.h"

IntrinsicCalibration::IntrinsicCalibration(Mat cameraMatrix, Mat distortionCoeffs, double reprojectionError)
{
	this->cameraMatrix = cameraMatrix;
	this->distortionCoeffs = distortionCoeffs;
	this->reprojectionError = reprojectionError;
}

Mat IntrinsicCalibration::getCameraMatrix()
{
	return cameraMatrix;
}

Mat IntrinsicCalibration::getDistortionCoeffs()
{
	return distortionCoeffs;
}

double IntrinsicCalibration::getReprojectionError()
{
	return reprojectionError;
}
