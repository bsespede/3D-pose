#include "Extrinsics.h"

Extrinsics::Extrinsics(Mat translationVector, Mat rotationVector, double reprojectionError)
{
	assert(translationVector.cols == 1);
	assert(rotationVector.cols == 1);
	assert(translationVector.rows == 3);
	assert(rotationVector.rows == 3);

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
