#include "ExtrinsicCalibration.h"

ExtrinsicCalibration::ExtrinsicCalibration(Mat translationMatrix, Mat rotationMatrix)
{
	this->translationMatrix = translationMatrix;
	this->rotationMatrix = rotationMatrix;
}

Mat ExtrinsicCalibration::getTranslationMatrix()
{
	return translationMatrix;
}

Mat ExtrinsicCalibration::getRotationMatrix()
{
	return rotationMatrix;
}