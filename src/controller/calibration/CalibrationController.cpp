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
	vector<vector<int>> allCharucoIds;
	vector<vector<Point2f>> allCharucoCorners;

	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	for (int frameNumber = 0; frameNumber < maxCheckboards; frameNumber++)
	{
		Mat frame = imread(checkboardsPath + "/" + to_string(frameNumber) + ".png");
		Mat result = frame.clone();
		frameSize = frame.size();

		vector<int> arucoIds;
		vector<vector<Point2f>> arucoCorners;
		aruco::detectMarkers(frame, dictionary, arucoCorners, arucoIds, params);

		if (arucoIds.size() > 0)
		{
			vector<int> charucoIds;
			vector<cv::Point2f> charucoCorners;
			aruco::interpolateCornersCharuco(arucoCorners, arucoIds, frame, board, charucoCorners, charucoIds);
			
			if (charucoIds.size() > 4)
			{
				string resultFolder = checkboardsPath + "/corners";
				filesystem::create_directory(resultFolder);				

				aruco::drawDetectedMarkers(result, arucoCorners);
				aruco::drawDetectedCornersCharuco(result, charucoCorners, charucoIds, Scalar(0, 0, 255));				
				imwrite(resultFolder + "/" + to_string(frameNumber) + ".png", result);

				allCharucoIds.push_back(charucoIds);
				allCharucoCorners.push_back(charucoCorners);
			}
			else
			{
				BOOST_LOG_TRIVIAL(warning) << "Not enough corners in frame " << frameNumber;
			}
		}
	}

	Mat cameraMatrix;
	Mat distortionCoeffs;
	
	double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs);
	IntrinsicCalibration* calibrationResult = new IntrinsicCalibration(cameraMatrix, distortionCoeffs, reprojectionError);

	return calibrationResult;
}