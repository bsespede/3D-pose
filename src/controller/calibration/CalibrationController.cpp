#include "CalibrationController.h"

CalibrationController::CalibrationController(Config* config)
{
	this->dictionary = aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	this->board = aruco::CharucoBoard::create(config->getCheckboardCols(), config->getCheckboardRows(), config->getCheckboardSquareLength(), config->getCheckboardMarkerLength(), dictionary);
	this->checkboardName = config->getCheckboardName();
	this->checkboardWidth = config->getCheckboardWidth();
	this->checkboardHeight = config->getCheckboardHeight();
	this->checkboardMargin = config->getCheckboardMargin();
	this->maxCheckboards = config->getMaxCheckboards();
}

int CalibrationController::getMaxCheckboards()
{
	return maxCheckboards;
}

void CalibrationController::generateCheckboard(string outputFolder)
{
	Mat boardImage;
	board->draw(cv::Size(checkboardWidth, checkboardHeight), boardImage, checkboardMargin, 1);
	imwrite(outputFolder + "/" + checkboardName + ".png", boardImage);
}

vector<IntrinsicCalibration> CalibrationController::calculateIntrinsics(vector<string> camerasPath)
{
	vector<IntrinsicCalibration> calibrationMatrices;

	for (int cameraNumber = 0; cameraNumber < camerasPath.size(); cameraNumber++)
	{		
		string cameraPath = camerasPath[cameraNumber];
		printf("Processing %s\n", camerasPath[cameraNumber].c_str());

		Size frameSize;
		vector<vector<int>> allCharucoIds;
		vector<vector<vector<Point2f>>> allCharucoCorners;
		vector<Mat> allFrames;

		Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();

		for (int frameNumber = 0; frameNumber < maxCheckboards - 1; frameNumber++)
		{
			Mat frame = imread(cameraPath + "/" + to_string(frameNumber) + ".png");
			Mat result = frame.clone();

			vector<int> charucoIds;
			vector<vector<Point2f>> charucoCorners;
			vector<vector<Point2f>> rejected;

			aruco::detectMarkers(frame, dictionary, charucoCorners, charucoIds, params, rejected);
			aruco::refineDetectedMarkers(frame, board, charucoCorners, charucoIds, rejected);

			aruco::drawDetectedMarkers(result, charucoCorners, charucoIds);
			imwrite(cameraPath + "/corners-" + to_string(frameNumber) + ".png", result);

			frameSize = frame.size();
			allCharucoIds.push_back(charucoIds);
			allCharucoCorners.push_back(charucoCorners);
			allFrames.push_back(frame);
		}

		Mat cameraMatrix;
		Mat distortionCoeffs;
		vector<Mat> interpolatedCharucoCorners;
		vector<Mat> interpolatedCharucoIds;

		for (int i = 0; i < maxCheckboards - 1; i++)
		{
			Mat currentCharucoCorners, currentCharucoIds;

			aruco::interpolateCornersCharuco(allCharucoCorners[i], allCharucoIds[i], allFrames[i], board, currentCharucoCorners, currentCharucoIds);
			interpolatedCharucoCorners.push_back(currentCharucoCorners);
			interpolatedCharucoIds.push_back(currentCharucoIds);
		}

		double reprojectionError = aruco::calibrateCameraCharuco(interpolatedCharucoCorners, interpolatedCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs);
		IntrinsicCalibration calibrationResult = IntrinsicCalibration(cameraMatrix, distortionCoeffs, reprojectionError);

		printf("RE: %f\n", reprojectionError);
		calibrationMatrices.push_back(calibrationResult);
	}

	return calibrationMatrices;
}