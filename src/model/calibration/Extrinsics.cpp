#include "Extrinsics.h"

Extrinsics::Extrinsics(cv::Mat translationVector, cv::Mat rotationVector, double reprojectionError)
{
	this->translationVector = translationVector;
	this->rotationVector = rotationVector;
	this->reprojectionError = reprojectionError;
}

cv::Mat Extrinsics::getTranslationVector()
{
	return translationVector;
}

cv::Mat Extrinsics::getRotationVector()
{
	return rotationVector;
}

double Extrinsics::getReprojectionError()
{
	return reprojectionError;
}
