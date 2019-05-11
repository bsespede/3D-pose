#include "IntrinsicCalibration.h"

IntrinsicCalibration::IntrinsicCalibration(Mat cameraMatrix, Mat distortionCoefficients, double reprojectionError)
{
	this->cameraMatrix = cameraMatrix;
	this->distortionCoefficients = distortionCoefficients;
	this->reprojectionError = reprojectionError;
}

Mat IntrinsicCalibration::getCameraMatrix()
{
	return cameraMatrix;
}

Mat IntrinsicCalibration::getDistortionCoefficients()
{
	return distortionCoefficients;
}

double IntrinsicCalibration::getReprojectionError()
{
	return reprojectionError;
}
