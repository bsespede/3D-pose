#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include "model/calibration/CalibrationType.h"
#include "model/scene/SceneController.h"
#include "model/config/ConfigController.h"

class CalibrationController
{
public:
	CalibrationController(ConfigController* configController, SceneController* sceneController);
	bool calibrate(Scene scene, CalibrationType calibrationType);
	void generateCheckboard(int charucoWidth, int charucoHeight, int charucoMargin);
private:
	bool calculateIntrinsics(Scene scene, CaptureType captureType);
	bool calculateExtrinsics(Scene scene, CaptureType captureType);
	bool calculatePoses(Scene scene, CaptureType captureType);
	bool detectCharucoCorners(cv::Mat& inputImage, int minCharucoCorners, std::vector<int>& charucoIds, std::vector<cv::Point2f>& charucoCorners);
	bool detectCharucoCorners(cv::Mat& inputImage, int minCharucoCorners, cv::Mat& cameraMatrix, cv::Mat& distortionCoefficients, std::vector<int>& charucoIds, std::vector<cv::Point2f>& charucoCorners);
	cv::Point3d fromModelToWorld(cv::Point3d modelPoint, cv::Mat cameraRotationVector, cv::Mat cameraTranslationVector, cv::Mat worldRotationVector, cv::Mat worldTranslationVector);
	SceneController* sceneController;	
	bool shouldOutputDebugData;
	int maxSamplesIntrinsics;
	int maxSamplesExtrinsics;
	cv::Ptr<cv::aruco::Dictionary> dictionary;
	cv::Ptr<cv::aruco::CharucoBoard> board;
};