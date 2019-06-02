#include "Extrinsics.h"

Extrinsics::Extrinsics(Mat translationVector, Mat rotationVector, double reprojectionError)
{
	this->translationVector = translationVector;
	this->rotationVector = rotationVector;
	this->reprojectionError = reprojectionError;
}

Mat Extrinsics::getTranslationVector()
{
	return translationVector;
}

Mat Extrinsics::getRotationVector()
{
	return rotationVector;
}

double Extrinsics::getReprojectionError()
{
	return reprojectionError;
}
