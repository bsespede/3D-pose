#pragma once

#include <opencv2/core.hpp>

using namespace cv;

class Extrinsics
{
public:
	Extrinsics(Mat translationMatrix, Mat rotationMatrix);
	Mat getTranslationMatrix();
	Mat getRotationMatrix();
private:
	Mat translationMatrix;
	Mat rotationMatrix;
};
