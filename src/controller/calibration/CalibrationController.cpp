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
		int cameraNumber = capturedCameras[cameraIndex];
		int maxCheckboards = fileController->getMaxCheckboards();

		Size frameSize;
		vector<vector<int>> allCharucoIds;
		vector<vector<Point2f>> allCharucoCorners;
		Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
		params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

		for (int frameNumber = 0; frameNumber < maxCheckboards; frameNumber++)
		{

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

		Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
		Mat distortionCoeffs;
		int calibrationFlags = 0 | CALIB_FIX_ASPECT_RATIO | CALIB_FIX_PRINCIPAL_POINT;

		BOOST_LOG_TRIVIAL(warning) << "Calibrating camera " << cameraNumber;
		double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs, noArray(), noArray(), calibrationFlags);

		BOOST_LOG_TRIVIAL(warning) << "Finished calibrating camera " << cameraNumber;

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

	for (int cameraIndex = 0; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int neighborCameraIndex = (cameraIndex + 1) % (int)capturedCameras.size();

		int cameraOriginal = capturedCameras[cameraIndex];
		int cameraNeighbor = capturedCameras[neighborCameraIndex];

		vector<vector<Point3f>> allObjects;
		vector<vector<Point3f>> allCornersOriginal;
		vector<vector<Point3f>> allCornersNeighbor;
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			Mat frameOriginal = fileController->getCapturedFrame(scene, operation, cameraOriginal, frameNumber);

			vector<int> arucoIdsOriginal;
			vector<vector<Point2f>> arucoCornersOriginal;
			Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
			params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

			aruco::detectMarkers(frameOriginal, dictionary, arucoCornersOriginal, arucoIdsOriginal, params);


			// get cornrs in cameraIndex
			// get corners in cameraNeighbor

			// set<int> uniqueIds
			// for ids in cameraIndexIds
				// for cameraNeighborIds
					// if cameraIndexIds == cameraNeighborIds
						// add id to uniqueIds
						// break

			// vector<3f> objectsInFrame
			// vector<2f> cornerFromCameraInFrame
			// vector<2f> cornerFromNeighborInFrame
			// for uniqueId
				// point3f = calculate position in board
				// poin2f1 = position of corner of uniqueId in left img
				// poin2f2 = position of corner of uniqueId in right img
				// add point3f to objectsInFrame
				// add point2f1 to cornerFromCameraInFrame
				// add point2f2 to cornerFromNeighborInFrame

			// add objectsInFrame to allObjects
			// add cornersFromCamera to allCornersInCamera
			// add cornersFromNeighbor to allCornersInNeighbor

		}

		if (arucoIds.size() > 0)
		{
			Intrinsics* intrinsics = fileController->getIntrinsics(cameraNumber);
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

				
			}
			else
			{
				BOOST_LOG_TRIVIAL(warning) << "Not enough corners in extrinsics calibration frame of camera " << cameraNumber;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "Not enough markers in extrinsics calibration frame " << cameraNumber;
		}
	}

	calibrationResults[cameraNumber] = new Extrinsics(translationVector, rotationVector);

	fileController->saveExtrinsics(scene, calibrationResults);
	return true;
}

void CalibrationController::detectCharucoCorners(Mat frame, vector<vector<int>> &allCharucoIds, vector<vector<Point2f>> &allCharucoCorners)
{
	
}