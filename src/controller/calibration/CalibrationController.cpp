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

map<int, IntrinsicCalibration*> CalibrationController::calculateIntrinsics(map<int, string> capturedCameras)
{
	map<int, IntrinsicCalibration*> calibrationMatrices;

	for (pair<int, string> cameraData: capturedCameras)
	{
		int cameraId = cameraData.first;
		string cameraPath = cameraData.second;

		BOOST_LOG_TRIVIAL(warning) << "Calibrating camera #" << cameraId;
		IntrinsicCalibration* calibrationResults = calculateIntrinsics(cameraPath);
		calibrationMatrices[cameraId] = calibrationResults;
	}

	return calibrationMatrices;
}

IntrinsicCalibration* CalibrationController::calculateIntrinsics(string checkboardsPath)
{
	Size frameSize;
	vector<vector<int>> allArucoIds;
	vector<vector<vector<Point2f>>> allArucoCorners;
	vector<Mat> allFrames;

	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();

	for (int frameNumber = 0; frameNumber < maxCheckboards; frameNumber++)
	{
		Mat frame = imread(checkboardsPath + "/" + to_string(frameNumber) + ".png");

		vector<int> arucoIds;
		vector<vector<Point2f>> arucoCorners;
		vector<vector<Point2f>> rejected;

		aruco::detectMarkers(frame, dictionary, arucoCorners, arucoIds, params);

		frameSize = frame.size();
		allArucoIds.push_back(arucoIds);
		allArucoCorners.push_back(arucoCorners);
		allFrames.push_back(frame);
	}

	Mat cameraMatrix;
	Mat distortionCoeffs;
	
	vector<vector<Point2f>> allCornersConcatenated;
	vector<int> allIdsConcatenated;
	vector<int> markerCounterPerFrame;
	markerCounterPerFrame.reserve(allArucoCorners.size());

	for (unsigned int i = 0; i < allArucoCorners.size(); i++)
	{
		markerCounterPerFrame.push_back((int)allArucoCorners[i].size());

		for (unsigned int j = 0; j < allArucoCorners[i].size(); j++)
		{
			allCornersConcatenated.push_back(allArucoCorners[i][j]);
			allIdsConcatenated.push_back(allArucoIds[i][j]);
		}
	}

	aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated, markerCounterPerFrame, board, frameSize, cameraMatrix, distortionCoeffs);

	vector<Mat> interpolatedCharucoCorners;
	vector<Mat> interpolatedCharucoIds;

	for (int frameNumber = 0; frameNumber < maxCheckboards; frameNumber++)
	{
		Mat currentCharucoCorners, currentCharucoIds;

		aruco::interpolateCornersCharuco(allArucoCorners[frameNumber], allArucoIds[frameNumber], allFrames[frameNumber], board, currentCharucoCorners, currentCharucoIds);
		interpolatedCharucoCorners.push_back(currentCharucoCorners);
		interpolatedCharucoIds.push_back(currentCharucoIds);

		Mat result = allFrames[frameNumber].clone();
		string resultFolder = checkboardsPath + "/corners";

		filesystem::create_directory(resultFolder);
		aruco::drawDetectedMarkers(result, allArucoCorners[frameNumber], allArucoIds[frameNumber]);		
		imwrite(resultFolder + "/" + to_string(frameNumber) + ".png", result);
	}

	double reprojectionError = aruco::calibrateCameraCharuco(interpolatedCharucoCorners, interpolatedCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs);
	IntrinsicCalibration* calibrationResult = new IntrinsicCalibration(cameraMatrix, distortionCoeffs, reprojectionError);

	return calibrationResult;
}