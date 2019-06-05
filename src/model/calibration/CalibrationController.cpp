#include "CalibrationController.h"

CalibrationController::CalibrationController(ConfigController* configController, SceneController* sceneController)
{
	int charucoCols = configController->getCharucoCols();
	int charucoRows = configController->getCharucoRows();
	float charucoSquareLength = configController->getCharucoSquareLength();
	float charucoMarkerLength = configController->getCharucoMarkerLength();

	this->sceneController = sceneController;
	this->dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	this->board = cv::aruco::CharucoBoard::create(charucoCols, charucoRows, charucoSquareLength, charucoMarkerLength, dictionary);
}

void CalibrationController::generateCheckboard(int charucoWidth, int charucoHeight, int charucoMargin)
{
	cv::Mat boardImage;
	board->draw(cv::Size(charucoWidth, charucoHeight), boardImage, charucoMargin, 1);
	cv::imwrite("board.png", boardImage);
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
	else
	{
		calibrationSuccess = calculatePoses(scene, CaptureType::CALIBRATION);
	}

	return calibrationSuccess;
}

bool CalibrationController::calculateIntrinsics(Scene scene, CaptureType captureType)
{
	std::map<int, Intrinsics*> intrinsics;
	int capturedFrames = sceneController->getCapturedFrames(scene, captureType);
	std::vector<int> capturedCameras = sceneController->getCapturedCameras(scene, captureType);

	#pragma omp parallel for
	for (int cameraIndex = 0; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		cv::Size frameSize;
		int cameraNumber = capturedCameras[cameraIndex];
		
		std::vector<std::vector<int>> allCharucoIds;
		std::vector<std::vector<cv::Point2f>> allCharucoCorners;
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			std::vector<int> charucoIds = std::vector<int>();
			std::vector<cv::Point2f> charucoCorners = std::vector<cv::Point2f>();
			cv::Mat frame = sceneController->getFrame(scene, captureType, cameraNumber, frameNumber);
			frameSize = frame.size();
			
			if (detectCharucoCorners(frame, 5, charucoIds, charucoCorners))
			{
				allCharucoIds.push_back(charucoIds);
				allCharucoCorners.push_back(charucoCorners);		
			}			
		}

		cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
		cv::Mat distortionCoeffs = cv::Mat::zeros(1, 5, CV_64F);
		int calibrationFlags = cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_FIX_PRINCIPAL_POINT | cv::CALIB_ZERO_TANGENT_DIST;
		
		double reprojectionError = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs, cv::noArray(), cv::noArray(), calibrationFlags);
		intrinsics[cameraNumber] = new Intrinsics(cameraMatrix, distortionCoeffs, reprojectionError);
	}

	sceneController->saveIntrinsics(scene, intrinsics);
	return true;
}

bool CalibrationController::calculateExtrinsics(Scene scene, CaptureType captureType)
{
	int capturedFrames = sceneController->getCapturedFrames(scene, captureType);
	std::vector<int> capturedCameras = sceneController->getCapturedCameras(scene, captureType);

	std::map<int, Extrinsics*> extrinsics;
	std::map<int, Intrinsics*> intrinsics = sceneController->getIntrinsics(scene);

	int originCamera = 0;
	cv::Mat originCameraMatrix = intrinsics[originCamera]->getCameraMatrix();
	cv::Mat originDistortionCoefficients = intrinsics[originCamera]->getDistortionCoefficients();

	bool foundPose = false;
	std::vector<int> charucoIds;
	std::vector<cv::Point2f> charucoCorners;
	cv::Mat frame = sceneController->getFrame(scene, captureType, originCamera, capturedFrames - 1);

	if (detectCharucoCorners(frame, 2, originCameraMatrix, originDistortionCoefficients, charucoIds, charucoCorners))
	{
		BOOST_LOG_TRIVIAL(warning) << "Found board in the floor of camera " << originCamera;

		cv::Mat originRotationVector;
		cv::Mat originTranslationVector;
		foundPose = cv::aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, originCameraMatrix, originDistortionCoefficients, originRotationVector, originTranslationVector);

		if (foundPose)
		{
			cv::Mat originRotationMatrix;
			cv::Rodrigues(originRotationVector, originRotationMatrix);

			cv::Mat cameraRotation = originRotationMatrix.t();
			cv::Mat cameraTranslation = (originRotationMatrix.t() * originTranslationVector) * (-1);

			extrinsics[originCamera] = new Extrinsics(cameraTranslation, cameraRotation, intrinsics[originCamera]->getReprojectionError());
		}		
	}

	if (!foundPose)
	{
		BOOST_LOG_TRIVIAL(warning) << "Did not find board in the floor of camera " << originCamera;

		cv::Mat originTranslationVector = cv::Mat::zeros(3, 1, CV_64F);
		cv::Mat originRotationVector = cv::Mat::zeros(3, 1, CV_64F);
		extrinsics[originCamera] = new Extrinsics(originTranslationVector, originRotationVector, intrinsics[originCamera]->getReprojectionError());
	}

	#pragma omp parallel for
	for (int cameraIndex = 1; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int cameraLeft = capturedCameras[cameraIndex - 1];
		int cameraRight = capturedCameras[cameraIndex];

		BOOST_LOG_TRIVIAL(warning) << "Calibrating for pair " << cameraLeft << "->" << cameraRight;

		int totalSamples = 0;		
		cv::Size cameraSize = sceneController->getFrame(scene, captureType, cameraLeft, 0).size();
		cv::Mat cameraMatrixLeft = intrinsics[cameraLeft]->getCameraMatrix();
		cv::Mat cameraMatrixRight = intrinsics[cameraRight]->getCameraMatrix();
		cv::Mat distortionCoefficientsLeft = intrinsics[cameraLeft]->getDistortionCoefficients();
		cv::Mat distortionCoefficientsRight = intrinsics[cameraRight]->getDistortionCoefficients();

		std::vector<std::vector<cv::Point3f>> allObjects;
		std::vector<std::vector<cv::Point2f>> allCornersLeft;
		std::vector<std::vector<cv::Point2f>> allCornersRight;
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			std::vector<int> idsLeft;
			std::vector<cv::Point2f> cornersLeft;
			cv::Mat frameLeft = sceneController->getFrame(scene, captureType, cameraLeft, frameNumber);
			if (!detectCharucoCorners(frameLeft, 4, cameraMatrixLeft, distortionCoefficientsLeft, idsLeft, cornersLeft))
			{
				continue;
			}

			std::vector<int> idsRight;
			std::vector<cv::Point2f> cornersRight;
			cv::Mat frameRight = sceneController->getFrame(scene, captureType, cameraRight, frameNumber);
			if (!detectCharucoCorners(frameRight, 4, cameraMatrixLeft, distortionCoefficientsRight, idsRight, cornersRight))
			{
				continue;
			}

			std::vector<cv::Point3f> finalObjects;
			std::vector<cv::Point2f> finalCornersRight;
			std::vector<cv::Point2f> finalCornersLeft;
			for (int leftIdIndex = 0; leftIdIndex < (int)idsLeft.size(); leftIdIndex++)
			{
				for (int rightIdIndex = 0; rightIdIndex < (int)idsRight.size(); rightIdIndex++)
				{
					int idLeft = idsLeft[leftIdIndex];
					int idRight = idsRight[rightIdIndex];

					if (idLeft == idRight)
					{
						cv::Point3f idPosition = board->chessboardCorners[idLeft];
						cv::Point2f leftPosition = cv::Point2f(cornersLeft[leftIdIndex].x, cornersLeft[leftIdIndex].y);
						cv::Point2f rightPosition = cv::Point2f(cornersRight[rightIdIndex].x, cornersRight[rightIdIndex].y);

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

		cv::Mat translationVector;
		cv::Mat rotationVector;
		cv::Mat essentialMatrix;
		cv::Mat fundamentalMatrix;
		double reprojectionError = cv::stereoCalibrate(allObjects, allCornersLeft, allCornersRight, cameraMatrixLeft, distortionCoefficientsLeft, cameraMatrixRight, distortionCoefficientsRight, cameraSize, rotationVector, translationVector, essentialMatrix, fundamentalMatrix);
		
		cv::Rodrigues(rotationVector, rotationVector);
		extrinsics[cameraRight] = new Extrinsics(translationVector, rotationVector, reprojectionError);

		BOOST_LOG_TRIVIAL(warning) << "Finished calibrating for pair " << cameraLeft << "->" << cameraRight;
	}

	for (int cameraNumber = 1; cameraNumber < (int)extrinsics.size(); cameraNumber++)
	{
		cv::Mat composedTranslationVector;
		cv::Mat composedRotationVector;
		cv::Mat currentTranslationVector = extrinsics[cameraNumber]->getTranslationVector();
		cv::Mat currentRotationVector = extrinsics[cameraNumber]->getRotationVector();
		cv::Mat previousTranslationVector = extrinsics[cameraNumber - 1]->getTranslationVector();
		cv::Mat previousRotationVector = extrinsics[cameraNumber - 1]->getRotationVector();

		cv::composeRT(previousRotationVector, previousTranslationVector, currentRotationVector, currentTranslationVector, composedRotationVector, composedTranslationVector);
		extrinsics[cameraNumber] = new Extrinsics(composedTranslationVector, composedRotationVector, extrinsics[cameraNumber]->getReprojectionError());
	}

	sceneController->saveExtrinsics(scene, extrinsics);
	return true;
}

bool CalibrationController::calculatePoses(Scene scene, CaptureType captureType)
{
	int capturedFrames = sceneController->getCapturedFrames(scene, captureType);
	std::vector<int> capturedCameras = sceneController->getCapturedCameras(scene, captureType);

	std::map<int, Intrinsics*> intrinsics = sceneController->getIntrinsics(scene);
	std::map<int, Extrinsics*> extrinsics = sceneController->getExtrinsics(scene);
	std::vector<Frame3D*> allPoses;

	for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
	{
		BOOST_LOG_TRIVIAL(warning) << "Calculating poses for cameras of frame " << frameNumber;
		Frame3D* framePoses = new Frame3D();
		bool foundPoses = false;

		#pragma omp parallel for
		for (int cameraIndex = 0; cameraIndex < capturedCameras.size(); cameraIndex++)
		{
			int cameraNumber = capturedCameras[cameraIndex];
			cv::Mat cameraMatrix = intrinsics[cameraNumber]->getCameraMatrix();
			cv::Mat distortionCoefficients = intrinsics[cameraNumber]->getDistortionCoefficients();
			
			std::vector<int> charucoIds;
			std::vector<cv::Point2f> charucoCorners;
			cv::Mat frame = sceneController->getFrame(scene, captureType, cameraNumber, frameNumber);

			if (detectCharucoCorners(frame, 1, cameraMatrix, distortionCoefficients, charucoIds, charucoCorners))
			{
				cv::Mat rotationVector;
				cv::Mat translationVector;
				if (cv::aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraMatrix, distortionCoefficients, rotationVector, translationVector))
				{
					cv::Mat composedRotationVector;
					cv::Mat composedTranslationVector;
					cv::Mat composedRotationMatrix;
					cv::Mat cameraRotationVector = extrinsics[cameraNumber]->getRotationVector();
					cv::Mat cameraTranslationVector = extrinsics[cameraNumber]->getTranslationVector();

					cv::composeRT(cameraRotationVector, cameraTranslationVector, rotationVector, translationVector, composedRotationVector, composedTranslationVector);
					cv::Rodrigues(composedRotationVector, composedRotationMatrix);

					std::list<cv::Point3d> composedPoses;
					cv::Affine3d affineMatrix = cv::Affine3d(composedRotationMatrix, composedTranslationVector);
					for (int cornerIndex = 0; cornerIndex < board->chessboardCorners.size(); cornerIndex++)
					{
						cv::Mat homogeneousPoint = cv::Mat(board->chessboardCorners[cornerIndex]);
						homogeneousPoint.convertTo(homogeneousPoint, CV_64F);
						homogeneousPoint.push_back(1.0);

						cv::Point3d transformedPoint = cv::Mat(affineMatrix.matrix * homogeneousPoint, cv::Range(0, 3));
						composedPoses.push_back(transformedPoint);
					}

					if (!composedPoses.empty())
					{
						foundPoses = true;
						framePoses->addData(cameraNumber, composedPoses);
					}
				}
			}
		}

		if (!foundPoses)
		{
			delete framePoses;
			allPoses.push_back(nullptr);
		}
		else
		{
			allPoses.push_back(framePoses);
		}		
	}

	BOOST_LOG_TRIVIAL(warning) << "Dumping all the poses to disk";
	sceneController->savePoses(scene, captureType, allPoses);
	return true;
}

bool CalibrationController::detectCharucoCorners(cv::Mat& inputImage, int minCharucoCorners, std::vector<int>& charucoIds, std::vector<cv::Point2f>& charucoCorners)
{
	cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
	params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;

	std::vector<int> arucoIds;
	std::vector<std::vector<cv::Point2f>> arucoCorners;
	cv::aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params);

	if (arucoIds.size() > 0)
	{
		cv::aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds);

		if (charucoIds.size() >= minCharucoCorners)
		{
			return true;
		}
	}

	return false;
}

bool CalibrationController::detectCharucoCorners(cv::Mat& inputImage, int minCharucoCorners, cv::Mat& cameraMatrix, cv::Mat& distortionCoefficients, std::vector<int>& charucoIds, std::vector<cv::Point2f>& charucoCorners)
{
	cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
	params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;

	std::vector<int> arucoIds;
	std::vector<std::vector<cv::Point2f>> arucoCorners;
	cv::aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params, cv::noArray(), cameraMatrix, distortionCoefficients);

	if (arucoIds.size() > 0)
	{
		cv::aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds, cameraMatrix, distortionCoefficients);

		if (charucoIds.size() >= minCharucoCorners)
		{
			return true;
		}
	}

	return false;
}