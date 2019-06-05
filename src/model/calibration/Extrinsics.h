#pragma once

#include <opencv2/core.hpp>

class Extrinsics
{
public:
	Extrinsics(cv::Mat translationVector, cv::Mat rotationVector, double reprojectionError);
	cv::Mat getTranslationVector();
	cv::Mat getRotationVector();
	double getReprojectionError();
private:
	cv::Mat translationVector;
	cv::Mat rotationVector;
	double reprojectionError;
};
