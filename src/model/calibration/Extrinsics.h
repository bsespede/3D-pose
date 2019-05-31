#pragma once

#include <opencv2/core.hpp>

using namespace cv;

class Extrinsics
{
public:
	Extrinsics(Mat translationVector, Mat rotationVector, double reprojectionError);
	Mat getTranslationVector();
	Mat getRotationVector();
	double getReprojectionError();
private:
	Mat translationVector;
	Mat rotationVector;
	double reprojectionError;
};
