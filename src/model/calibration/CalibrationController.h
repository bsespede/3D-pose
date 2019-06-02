#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include "model/scene/Scene.h"
#include "model/calibration/parameters/Intrinsics.h"
#include "model/calibration/parameters/Extrinsics.h"
#include "model/scene/SceneController.h"
#include "model/config/ConfigController.h"

using namespace std;
using namespace boost;
using namespace cv;

class CalibrationController
{
public:
	CalibrationController(ConfigController* configController, SceneController* sceneController);
	bool calibrate(Scene scene, Operation operation);
	void generateCheckboard();		
private:
	bool calculateIntrinsics(Scene scene, Operation operation);
	bool calculateExtrinsics(Scene scene, Operation operation);
	bool detectCharucoCorners(Mat& inputImage, int minCharucoCorners, vector<int>& charucoIds, vector<Point2f>& charucoCorners);
	bool detectCharucoCorners(Mat& inputImage, int minCharucoCorners, Mat& cameraMatrix, Mat& distortionCoefficients, vector<int>& charucoIds, vector<Point2f>& charucoCorners);
	ConfigController* configController;
	SceneController* sceneController;
	Ptr<aruco::Dictionary> dictionary;
	Ptr<aruco::CharucoBoard> board;
};