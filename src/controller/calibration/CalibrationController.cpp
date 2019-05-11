#include "CalibrationController.h"

CalibrationController::CalibrationController(FileController* fileController)
{
	int checkboardCols = fileController->getCheckboardCols();
	int checkboardRows = fileController->getCheckboardRows();
	int checkboardSquareLength = fileController->getCheckboardSquareLength();
	int checkboardMarkerLength = fileController->getCheckboardMarkerLength();

	this->fileController = fileController;
	this->dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
	this->board = aruco::CharucoBoard::create(checkboardCols, checkboardRows, checkboardSquareLength, checkboardMarkerLength, dictionary);
}

void CalibrationController::generateCheckboard()
{
	Mat boardImage;
	string dataFolder = fileController->getDataFolder();
	int checkboardWidth = fileController->getCheckboardWidth();
	int checkboardHeight = fileController->getCheckboardHeight();
	int checkboardMargin = fileController->getCheckboardMargin();

	board->draw(Size(checkboardWidth, checkboardHeight), boardImage, checkboardMargin, 1);
	imwrite(dataFolder + "/board.png", boardImage);
}

bool CalibrationController::calibrate(Scene scene, Operation operation)
{
	if (!fileController->hasCapture(scene, operation))
	{
		BOOST_LOG_TRIVIAL(warning) << "Cannot calibrate scene " << scene.getName() << " since it has no " << operation.toString() << " capture or it is corrupted";
		return false;
	}

	bool calibrationSuccess;

	if (operation == Operation::INTRINSICS)
	{
		calibrationSuccess = calculateIntrinsics(scene, operation);
	} 
	else if (operation == Operation::EXTRINSICS)
	{
		calibrationSuccess = calculateExtrinsics(scene, operation);
	}

	return calibrationSuccess;
}

bool CalibrationController::calculateIntrinsics(Scene scene, Operation operation)
{
	map<int, IntrinsicCalibration*> calibrationResults;
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, operation);

	for (int cameraNumber: capturedCameras)
	{
		int maxCheckboards = fileController->getMaxCheckboards();

		Size frameSize;
		vector<vector<int>> allCharucoIds;
		vector<vector<Point2f>> allCharucoCorners;

		Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
		params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

		for (int frameNumber = 0; frameNumber < maxCheckboards; frameNumber++)
		{
			if (!fileController->hasCapturedFrame(scene, operation, cameraNumber, frameNumber))
			{
				BOOST_LOG_TRIVIAL(warning) << "Could not find captured frame " << frameNumber << " from camera " << cameraNumber << " for " << operation.toString();
				return false;
			}

			Mat frame = fileController->getCapturedFrame(scene, operation, cameraNumber, frameNumber);
			Mat result = frame.clone();
			frameSize = frame.size();

			vector<int> arucoIds;
			vector<vector<Point2f>> arucoCorners;
			aruco::detectMarkers(frame, dictionary, arucoCorners, arucoIds, params);

			if (arucoIds.size() > 0)
			{
				vector<int> charucoIds;
				vector<Point2f> charucoCorners;
				aruco::interpolateCornersCharuco(arucoCorners, arucoIds, frame, board, charucoCorners, charucoIds);

				if (charucoIds.size() > 4)
				{
					aruco::drawDetectedMarkers(result, arucoCorners);
					aruco::drawDetectedCornersCharuco(result, charucoCorners, charucoIds, Scalar(0, 0, 255));
					fileController->saveCalibrationDetections(result, scene, operation, cameraNumber, frameNumber);

					allCharucoIds.push_back(charucoIds);
					allCharucoCorners.push_back(charucoCorners);
				}
				else
				{
					BOOST_LOG_TRIVIAL(warning) << "Not enough corners, ignoring frame " << frameNumber << " of camera " << cameraNumber;
				}
			}
			else
			{
				BOOST_LOG_TRIVIAL(warning) << "Not enough markers, ignoring frame " << frameNumber << " of camera " << cameraNumber;
			}
		}

		Mat cameraMatrix;
		Mat distortionCoeffs;
		double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs);

		calibrationResults[cameraNumber] = new IntrinsicCalibration(cameraMatrix, distortionCoeffs, reprojectionError);		
	}

	fileController->saveIntrinsics(calibrationResults);
	return true;
}

bool CalibrationController::calculateExtrinsics(Scene scene, Operation operation)
{
	map<int, ExtrinsicCalibration*> calibrationResults;
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, operation);

	for (int cameraNumber : capturedCameras)
	{
		int frameNumber = 0;
		if (!fileController->hasCapturedFrame(scene, operation, cameraNumber, frameNumber))
		{
			BOOST_LOG_TRIVIAL(warning) << "Could not find captured frame from camera " << cameraNumber << " for " << operation.toString();
			return false;
		}

		vector<int> arucoIds;
		vector<vector<Point2f>> arucoCorners;
		Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
		params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

		Mat frame = fileController->getCapturedFrame(scene, operation, cameraNumber, frameNumber);
		Mat result = frame.clone();
		Size frameSize = frame.size();

		aruco::detectMarkers(frame, dictionary, arucoCorners, arucoIds, params);

		if (arucoIds.size() > 0)
		{
			IntrinsicCalibration* intrinsics = fileController->getIntrinsics(cameraNumber);
			if (intrinsics == nullptr)
			{
				BOOST_LOG_TRIVIAL(warning) << "No intrinsics could be found for camera " << cameraNumber;
				return false;
			}

			Mat cameraMatrix = intrinsics->getCameraMatrix();
			Mat distortionCoefficients = intrinsics->getDistortionCoefficients();

			vector<int> charucoIds;
			vector<Point2f> charucoCorners;
			aruco::interpolateCornersCharuco(arucoCorners, arucoIds, frame, board, charucoCorners, charucoIds, cameraMatrix, distortionCoefficients);

			if (charucoIds.size() > 4)
			{
				aruco::drawDetectedMarkers(result, arucoCorners);
				aruco::drawDetectedCornersCharuco(result, charucoCorners, charucoIds, Scalar(0, 0, 255));
				fileController->saveCalibrationDetections(result, scene, operation, cameraNumber, frameNumber);

				Mat translationVector;
				Mat rotationVector;
				bool poseEstimationSuccess = aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraMatrix, distortionCoefficients, rotationVector, translationVector);
				delete intrinsics;

				if (!poseEstimationSuccess)
				{
					BOOST_LOG_TRIVIAL(warning) << "Pose estimation failed during extrinsics calibration of camera " << cameraNumber;
					return false;
				}
				else
				{
					aruco::drawAxis(result, cameraMatrix, distortionCoefficients, rotationVector, translationVector, 0.1);
					fileController->saveCalibrationDetections(result, scene, operation, cameraNumber, frameNumber);
				}

				calibrationResults[cameraNumber] = new ExtrinsicCalibration(translationVector, rotationVector);
			}
			else
			{
				BOOST_LOG_TRIVIAL(warning) << "Not enough corners in extrinsics calibration frame";
				return false;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "Not enough markers in extrinsics calibration frame";
			return false;
		}
	}

	fileController->saveExtrinsics(scene, calibrationResults);
	return true;
}