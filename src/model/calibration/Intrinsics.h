#pragma once

#include <opencv2/core.hpp>

class Intrinsics
{
public:
	Intrinsics(cv::Mat cameraMatrix, cv::Mat distortionCoefficients, double reprojectionError);
	cv::Mat getCameraMatrix();
	cv::Mat getDistortionCoefficients();
	double getReprojectionError();
private:
	cv::Mat cameraMatrix;
	cv::Mat distortionCoefficients;
	double reprojectionError;
};
