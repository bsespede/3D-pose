#pragma once

#include <opencv2/core.hpp>

using namespace cv;

class IntrinsicCalibration
{
public:
	IntrinsicCalibration(Mat cameraMatrix, Mat distortionCoeffs, double reprojectionError);
	Mat getCameraMatrix();
	Mat getDistortionCoeffs();
	double getReprojectionError();
private:
	Mat cameraMatrix;
	Mat distortionCoeffs;
	double reprojectionError;
};
