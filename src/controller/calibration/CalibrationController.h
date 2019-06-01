#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include "model/scene/Scene.h"
#include "model/scene/enum/Operation.h"
#include "model/calibration/Intrinsics.h"
#include "model/calibration/Extrinsics.h"
#include "controller/files/FileController.h"

using namespace std;
using namespace boost;
using namespace cv;

class CalibrationController
{
public:
	CalibrationController(FileController* fileController);
	bool calibrate(Scene scene, Operation operation);
	void generateCheckboard();		
private:
	bool calculateIntrinsics(Scene scene, Operation operation);
	bool calculateExtrinsics(Scene scene, Operation operation);
	bool detectCharucoCorners(Mat& inputImage, Mat& outputImage, vector<int>& charucoIds, vector<Point2f>& charucoCorners);
	bool detectCharucoPose(Mat& inputImage, Mat& outputImage, Mat& cameraMatrix, Mat& distortionCoefficients, Mat& rotationVector, Mat& translationVector);
	void renderCalibration(Scene scene);
	FileController* fileController;
	Ptr<aruco::Dictionary> dictionary;
	Ptr<aruco::CharucoBoard> board;
};