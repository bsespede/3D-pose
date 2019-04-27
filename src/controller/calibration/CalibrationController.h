#pragma once

#include <string>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "model/calibration/IntrinsicCalibration.h"

using namespace std;
using namespace cv;

class CalibrationController
{
public:
	CalibrationController(int maxCheckboards);	
	void printCheckboard(string pathToOutput);
	IntrinsicCalibration calculateIntrinsics(string pathToInput);
	int getMaxCheckboards();
private:
	int maxCheckboards;
	Ptr<aruco::Dictionary> dictionary;
	Ptr<aruco::CharucoBoard> board;
};