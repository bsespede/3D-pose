#include "CalibrationController.h"

CalibrationController::CalibrationController(int maxCheckboards)
{
	this->dictionary = aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	this->board = aruco::CharucoBoard::create(5, 7, 47.18f, 23.63f, dictionary);
	this->maxCheckboards = maxCheckboards;
}

int CalibrationController::getMaxCheckboards()
{
	return maxCheckboards;
}

void CalibrationController::printCheckboard(string pathToOutput)
{
	Mat boardImage;
	board->draw(cv::Size(1485, 2100), boardImage, 150, 1);
	imwrite(pathToOutput + "/board.png", boardImage);
}

IntrinsicCalibration CalibrationController::calculateIntrinsics(string pathToInput)
{
	Size frameSize;
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
		
	vector<vector<int>> allCharucoIds;
	vector<vector<Point2f>> allCharucoCorners;

	for (int frameNumber = 0; frameNumber < maxCheckboards; frameNumber++)
	{
		Mat frame = imread(pathToInput + "/" + to_string(frameNumber) + ".png");
		Mat result;

		vector<int> charucoIds;
		vector<Point2f> charucoCorners;
		aruco::detectMarkers(frame, dictionary, charucoCorners, charucoIds, params);

		aruco::drawDetectedMarkers(result, charucoCorners, charucoIds);
		imwrite(pathToInput + "/corners-" + to_string(frameNumber) + ".png", result);

		frameSize = frame.size();
		allCharucoIds.push_back(charucoIds);
		allCharucoCorners.push_back(charucoCorners);
	}
	
	Mat cameraMatrix, distortionCoeffs;
	double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs);
	IntrinsicCalibration calibrationResult = IntrinsicCalibration(cameraMatrix, distortionCoeffs, reprojectionError);
	
	return calibrationResult;
}