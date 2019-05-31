#include "CalibrationController.h"

CalibrationController::CalibrationController(FileController* fileController)
{
	int checkboardCols = fileController->getCheckboardCols();
	int checkboardRows = fileController->getCheckboardRows();
	double checkboardSquareLength = fileController->getCheckboardSquareLength();
	double checkboardMarkerLength = fileController->getCheckboardMarkerLength();

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

	bool calibrationSuccess = false;

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
	map<int, Intrinsics*> calibrationResults;
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, operation);

	#pragma omp parallel for
	for (int cameraIndex = 0; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		Size frameSize;
		int cameraNumber = capturedCameras[cameraIndex];
		int maxCheckboards = fileController->getMaxCheckboards();
		
		vector<vector<int>> allCharucoIds;
		vector<vector<Point2f>> allCharucoCorners;

		for (int frameNumber = 0; frameNumber < maxCheckboards; frameNumber++)
		{
			vector<int> charucoIds = vector<int>();
			vector<Point2f> charucoCorners = vector<Point2f>();
			Mat frame = fileController->getCapturedFrame(scene, operation, cameraNumber, frameNumber);
			Mat result = frame.clone();
			frameSize = frame.size();
			
			if (detectCharucoCorners(frame, result, charucoIds, charucoCorners))
			{
				allCharucoIds.push_back(charucoIds);
				allCharucoCorners.push_back(charucoCorners);		
			}
			else
			{
				BOOST_LOG_TRIVIAL(warning) << "Not enough charuco corners in frame " << frameNumber << " of camera " << cameraNumber;
			}			
		}

		Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
		Mat distortionCoeffs;
		int calibrationFlags = 0 | CALIB_FIX_ASPECT_RATIO | CALIB_FIX_PRINCIPAL_POINT;
		
		double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs, noArray(), noArray(), calibrationFlags);
		calibrationResults[cameraNumber] = new Intrinsics(cameraMatrix, distortionCoeffs, reprojectionError);		
	}

	fileController->saveIntrinsics(calibrationResults);
	return true;
}

bool CalibrationController::calculateExtrinsics(Scene scene, Operation operation)
{
	map<int, Extrinsics*> calibrationResults;
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, operation);
	int capturedFrames = fileController->getCapturedFrames(scene, operation);

	#pragma omp parallel for
	for (int cameraIndex = 0; cameraIndex < (int)capturedCameras.size() - 1; cameraIndex++)
	{
		int cameraLeft = capturedCameras[cameraIndex];
		int cameraRight = capturedCameras[cameraIndex + 1];
		Size cameraSize;
		BOOST_LOG_TRIVIAL(warning) << "Calibrating camera pair " << cameraLeft << "-" << cameraRight;

		vector<vector<Point3f>> allObjects;
		vector<vector<Point2f>> allCornersLeft;
		vector<vector<Point2f>> allCornersRight;
		int totalSamples = 0;
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			BOOST_LOG_TRIVIAL(warning) << "Processing frame " << frameNumber << " for camera pair " << cameraLeft << "-" << cameraRight;

			if (totalSamples > 250)
			{
				break;
			}

			vector<int> idsLeft;
			vector<Point2f> cornersLeft;
			Mat frameLeft = fileController->getCapturedFrame(scene, operation, cameraLeft, frameNumber);
			Mat frameLeftResult = frameLeft.clone();
			cameraSize = frameLeft.size();

			if (!detectCharucoCorners(frameLeft, frameLeftResult, idsLeft, cornersLeft))
			{
				continue;
			}

			vector<int> idsRight;
			vector<Point2f> cornersRight;
			Mat frameRight = fileController->getCapturedFrame(scene, operation, cameraRight, frameNumber);
			Mat frameRightResult = frameRight.clone();

			if (!detectCharucoCorners(frameRight, frameRightResult, idsRight, cornersRight))
			{
				continue;
			}

			vector<Point3f> finalObjects;
			vector<Point2f> finalCornersRight;
			vector<Point2f> finalCornersLeft;
			for (int leftIdIndex = 0; leftIdIndex < (int)idsLeft.size(); leftIdIndex++)
			{
				for (int rightIdIndex = 0; rightIdIndex < (int)idsRight.size(); rightIdIndex++)
				{
					int idLeft = idsLeft[leftIdIndex];
					int idRight = idsRight[rightIdIndex];

					if (idLeft == idRight)
					{
						int row = idLeft / board->getChessboardSize().width;
						int col = idLeft % board->getChessboardSize().width;
						float squareSize = board->getSquareLength();

						Point3f idPosition = Point3f((float)col * squareSize, (float)row * squareSize, 0);
						Point2f leftPosition = Point2f(cornersLeft[leftIdIndex].x, cornersLeft[leftIdIndex].y);
						Point2f rightPosition = Point2f(cornersRight[rightIdIndex].x, cornersRight[rightIdIndex].y);

						finalObjects.push_back(idPosition);
						finalCornersLeft.push_back(leftPosition);
						finalCornersRight.push_back(rightPosition);
					}
				}
			}

			if (finalObjects.size() > 3) 
			{
				allObjects.push_back(finalObjects);
				allCornersLeft.push_back(finalCornersLeft);
				allCornersRight.push_back(finalCornersRight);
				totalSamples += finalObjects.size();
			}
		}

		Intrinsics* leftIntrinsics = fileController->getIntrinsics(cameraLeft);
		Intrinsics* rightIntrinsics = fileController->getIntrinsics(cameraRight);
		Mat translationVector;
		Mat rotationVector;
		Mat essentialMatrix;
		Mat fundamentalMatrix;

		BOOST_LOG_TRIVIAL(warning) << "Calibrating camera pair " << cameraLeft << "-" << cameraRight;
		double reprojectionError = cv::stereoCalibrate(allObjects, allCornersLeft, allCornersRight, leftIntrinsics->getCameraMatrix(), leftIntrinsics->getDistortionCoefficients(), rightIntrinsics->getCameraMatrix(), rightIntrinsics->getDistortionCoefficients(), cameraSize, rotationVector, translationVector, essentialMatrix, fundamentalMatrix);
		calibrationResults[cameraLeft] = new Extrinsics(translationVector, rotationVector, reprojectionError);
		BOOST_LOG_TRIVIAL(warning) << "Finished calibrating camera pair " << cameraLeft << "-" << cameraRight;
	}

	fileController->saveExtrinsics(scene, calibrationResults);
	return true;
}

bool CalibrationController::detectCharucoCorners(Mat& input, Mat& output, vector<int>& charucoIds, vector<Point2f>& charucoCorners)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners;
	aruco::detectMarkers(input, dictionary, arucoCorners, arucoIds, params);

	if (arucoIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(arucoCorners, arucoIds, input, board, charucoCorners, charucoIds);

		if (charucoIds.size() > 4)
		{
			aruco::drawDetectedMarkers(output, arucoCorners);
			aruco::drawDetectedCornersCharuco(output, charucoCorners, charucoIds, Scalar(0, 0, 255));
			return true;
		}
	}
	
	return false;
}