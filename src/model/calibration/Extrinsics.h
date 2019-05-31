#pragma once

#include <opencv2/core.hpp>

using namespace cv;

class Extrinsics
{
public:
	Extrinsics(Mat translationMatrix, Mat rotationMatrix, double reprojectionError);
	Mat getTranslationMatrix();
	Mat getRotationMatrix();
	double getReprojectionError();
private:
	Mat translationMatrix;
	Mat rotationMatrix;
	double reprojectionError;
};
