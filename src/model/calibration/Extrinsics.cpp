#include "Extrinsics.h"

Extrinsics::Extrinsics(Mat translationMatrix, Mat rotationMatrix, double reprojectionError)
{
	this->translationMatrix = translationMatrix;
	this->rotationMatrix = rotationMatrix;
	this->reprojectionError = reprojectionError;
}

Mat Extrinsics::getTranslationMatrix()
{
	return translationMatrix;
}

Mat Extrinsics::getRotationMatrix()
{
	return rotationMatrix;
}

double Extrinsics::getReprojectionError()
{
	return reprojectionError;
}
