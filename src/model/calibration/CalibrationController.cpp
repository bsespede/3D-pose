#include "CalibrationController.h"

CalibrationController::CalibrationController(ConfigController* configController, SceneController* sceneController)
{
	int charucoCols = configController->getCharucoCols();
	int charucoRows = configController->getCharucoRows();
	float charucoSquareLength = configController->getCharucoSquareLength();
	float charucoMarkerLength = configController->getCharucoMarkerLength();

	this->sceneController = sceneController;
	this->dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
	this->board = aruco::CharucoBoard::create(charucoCols, charucoRows, charucoSquareLength, charucoMarkerLength, dictionary);
}

void CalibrationController::generateCheckboard(int charucoWidth, int charucoHeight, int charucoMargin)
{
	Mat boardImage;
	board->draw(Size(charucoWidth, charucoHeight), boardImage, charucoMargin, 1);
	imwrite("board.png", boardImage);
}

bool CalibrationController::calibrate(Scene scene, CalibrationType calibrationType)
{
	bool calibrationSuccess = false;

	if (calibrationType == CalibrationType::INTRINSICS)
	{
		calibrationSuccess = calculateIntrinsics(scene, CaptureType::CALIBRATION);
	} 
	else if (calibrationType == CalibrationType::EXTRINSICS)
	{
		calibrationSuccess = calculateExtrinsics(scene, CaptureType::CALIBRATION);
	}

	return calibrationSuccess;
}

bool CalibrationController::calculateIntrinsics(Scene scene, CaptureType captureType)
{
	map<int, Intrinsics*> intrinsics;
	int capturedFrames = sceneController->getCapturedFrameNumber(scene, captureType);
	vector<int> capturedCameras = sceneController->getCapturedCameras(scene, captureType);

	#pragma omp parallel for
	for (int cameraIndex = 0; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		Size frameSize;
		int cameraNumber = capturedCameras[cameraIndex];
		
		vector<vector<int>> allCharucoIds;
		vector<vector<Point2f>> allCharucoCorners;
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			vector<int> charucoIds = vector<int>();
			vector<Point2f> charucoCorners = vector<Point2f>();
			Mat frame = sceneController->getCapturedFrame(scene, captureType, cameraNumber, frameNumber);
			frameSize = frame.size();
			
			if (detectCharucoCorners(frame, 5, charucoIds, charucoCorners))
			{
				allCharucoIds.push_back(charucoIds);
				allCharucoCorners.push_back(charucoCorners);		
			}			
		}

		Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
		Mat distortionCoeffs = Mat::zeros(1, 5, CV_64F);
		int calibrationFlags = CALIB_FIX_ASPECT_RATIO | CALIB_FIX_PRINCIPAL_POINT | CALIB_ZERO_TANGENT_DIST;
		
		double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs, noArray(), noArray(), calibrationFlags);
		intrinsics[cameraNumber] = new Intrinsics(cameraMatrix, distortionCoeffs, reprojectionError);
	}

	sceneController->saveIntrinsics(scene, intrinsics);
	return true;
}

bool CalibrationController::calculateExtrinsics(Scene scene, CaptureType captureType)
{
	int capturedFrames = sceneController->getCapturedFrameNumber(scene, captureType);
	vector<int> capturedCameras = sceneController->getCapturedCameras(scene, captureType);

	map<int, Extrinsics*> extrinsics;
	map<int, Intrinsics*> intrinsics;
	for (int cameraNumber : capturedCameras)
	{
		intrinsics[cameraNumber] = sceneController->getIntrinsics(scene, cameraNumber);
	}	

	int originCamera = 0;
	Mat originCameraMatrix = intrinsics[originCamera]->getCameraMatrix();
	Mat originDistortionCoefficients = intrinsics[originCamera]->getDistortionCoefficients();

	vector<int> charucoIds;
	vector<Point2f> charucoCorners;
	Mat frame = sceneController->getCapturedFrame(scene, captureType, originCamera, capturedFrames - 1);
	Mat frameResult = frame.clone();
	if (detectCharucoCorners(frame, 1, originCameraMatrix, originDistortionCoefficients, charucoIds, charucoCorners))
	{
		BOOST_LOG_TRIVIAL(warning) << "Found board in the floor of camera " << originCamera;

		Mat originRotationVector;
		Mat originTranslationVector;
		aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, originCameraMatrix, originDistortionCoefficients, originRotationVector, originTranslationVector);
		extrinsics[originCamera] = new Extrinsics(originTranslationVector, originRotationVector, intrinsics[originCamera]->getReprojectionError());
	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "Did not find board in the floor of camera " << originCamera;

		Mat originTranslationVector = Mat::zeros(3, 1, CV_64F);
		Mat originRotationVector = Mat::zeros(3, 1, CV_64F);
		extrinsics[originCamera] = new Extrinsics(originTranslationVector, originRotationVector, 0.0);
	}

	#pragma omp parallel for
	for (int cameraIndex = 1; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int cameraLeft = capturedCameras[cameraIndex - 1];
		int cameraRight = capturedCameras[cameraIndex];

		BOOST_LOG_TRIVIAL(warning) << "Calibrating for pair " << cameraLeft << "->" << cameraRight;

		int totalSamples = 0;		
		Size cameraSize = sceneController->getCapturedFrame(scene, captureType, cameraLeft, 0).size();
		Mat cameraMatrixLeft = intrinsics[cameraLeft]->getCameraMatrix();
		Mat cameraMatrixRight = intrinsics[cameraRight]->getCameraMatrix();
		Mat distortionCoefficientsLeft = intrinsics[cameraLeft]->getDistortionCoefficients();		
		Mat distortionCoefficientsRight = intrinsics[cameraRight]->getDistortionCoefficients();

		vector<vector<Point3f>> allObjects;
		vector<vector<Point2f>> allCornersLeft;
		vector<vector<Point2f>> allCornersRight;		
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			vector<int> idsLeft;
			vector<Point2f> cornersLeft;
			Mat frameLeft = sceneController->getCapturedFrame(scene, captureType, cameraLeft, frameNumber);
			if (!detectCharucoCorners(frameLeft, 4, cameraMatrixLeft, distortionCoefficientsLeft, idsLeft, cornersLeft))
			{
				continue;
			}

			vector<int> idsRight;
			vector<Point2f> cornersRight;
			Mat frameRight = sceneController->getCapturedFrame(scene, captureType, cameraRight, frameNumber);
			if (!detectCharucoCorners(frameRight, 4, cameraMatrixLeft, distortionCoefficientsRight, idsRight, cornersRight))
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
						int row = idLeft / (board->getChessboardSize().width - 1);
						int col = idLeft % (board->getChessboardSize().width - 1);
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
				totalSamples += (int)finalObjects.size();
			}
		}

		BOOST_LOG_TRIVIAL(warning) << "Samples for pair " << cameraLeft << "->" << cameraRight << ": " << totalSamples;

		Mat translationVector;
		Mat rotationVector;
		Mat essentialMatrix;
		Mat fundamentalMatrix;
		double reprojectionError = cv::stereoCalibrate(allObjects, allCornersLeft, allCornersRight, cameraMatrixLeft, distortionCoefficientsLeft, cameraMatrixRight, distortionCoefficientsRight, cameraSize, rotationVector, translationVector, essentialMatrix, fundamentalMatrix);
		
		cv::Rodrigues(rotationVector, rotationVector);
		extrinsics[cameraRight] = new Extrinsics(translationVector, rotationVector, reprojectionError);

		BOOST_LOG_TRIVIAL(warning) << "Finished calibrating for pair " << cameraLeft << "->" << cameraRight;
	}

	for (int cameraNumber = 1; cameraNumber < (int)extrinsics.size(); cameraNumber++)
	{
		Mat composedTranslationVector;
		Mat composedRotationVector;
		Mat currentTranslationVector = extrinsics[cameraNumber]->getTranslationVector();
		Mat currentRotationVector = extrinsics[cameraNumber]->getRotationVector();
		Mat previousTranslationVector = extrinsics[cameraNumber - 1]->getTranslationVector();
		Mat previousRotationVector = extrinsics[cameraNumber - 1]->getRotationVector();

		cv::composeRT(previousRotationVector, previousTranslationVector, currentRotationVector, currentTranslationVector, composedRotationVector, composedTranslationVector);
		extrinsics[cameraNumber] = new Extrinsics(composedTranslationVector, composedRotationVector, extrinsics[cameraNumber]->getReprojectionError());
	}

	sceneController->saveExtrinsics(scene, extrinsics);
	return true;
}

bool CalibrationController::detectCharucoCorners(Mat& inputImage, int minCharucoCorners, vector<int>& charucoIds, vector<Point2f>& charucoCorners)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners;	
	aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params);

	if (arucoIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds);

		if (charucoIds.size() >= minCharucoCorners)
		{
			return true;
		}
	}

	return false;
}

bool CalibrationController::detectCharucoCorners(Mat& inputImage, int minCharucoCorners, Mat& cameraMatrix, Mat& distortionCoefficients, vector<int>& charucoIds, vector<Point2f>& charucoCorners)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners;
	aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params, noArray(), cameraMatrix, distortionCoefficients);

	if (arucoIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds, cameraMatrix, distortionCoefficients);

		if (charucoIds.size() >= minCharucoCorners)
		{
			return true;
		}
	}

	return false;
}