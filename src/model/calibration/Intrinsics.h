#pragma once

#include <opencv2/core.hpp>

using namespace cv;

class Intrinsics
{
public:
	Intrinsics(Mat cameraMatrix, Mat distortionCoefficients, double reprojectionError);
	Mat getCameraMatrix();
	Mat getDistortionCoefficients();
	double getReprojectionError();
private:
	Mat cameraMatrix;
	Mat distortionCoefficients;
	double reprojectionError;
};
