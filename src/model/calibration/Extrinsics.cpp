#include "Extrinsics.h"

Extrinsics::Extrinsics(Mat translationMatrix, Mat rotationMatrix)
{
	this->translationMatrix = translationMatrix;
	this->rotationMatrix = rotationMatrix;
}

Mat Extrinsics::getTranslationMatrix()
{
	return translationMatrix;
}

Mat Extrinsics::getRotationMatrix()
{
	return rotationMatrix;
}