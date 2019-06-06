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
	this->outputDetections = configController->getOutputDetections();
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
	for (int cameraIndex = 0; cameraIndex < capturedCameras.size(); cameraIndex++)
	{
		int cameraNumber = capturedCameras[cameraIndex];
		int totalSamples = 0;

		BOOST_LOG_TRIVIAL(warning) << "Calculating intrincs camera " << cameraNumber;

		cv::Size frameSize = sceneController->getFrame(scene, captureType, cameraNumber, 0).size();
		std::vector<std::vector<int>> allCharucoIds;
		std::vector<std::vector<cv::Point2f>> allCharucoCorners;
		
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			std::vector<int> charucoIds = std::vector<int>();
			std::vector<cv::Point2f> charucoCorners = std::vector<cv::Point2f>();
			cv::Mat frame = sceneController->getFrame(scene, captureType, cameraNumber, frameNumber);
			
			if (detectCharucoCorners(frame, 5, charucoIds, charucoCorners))
			{
				allCharucoIds.push_back(charucoIds);
				allCharucoCorners.push_back(charucoCorners);	
				totalSamples += (int)allCharucoCorners.size();
			}			
		}

		BOOST_LOG_TRIVIAL(warning) << "Samples intrinsics camera " << cameraNumber << ": " << totalSamples;

		cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
		cv::Mat distortionCoeffs = cv::Mat::zeros(1, 5, CV_64F);
		int calibrationFlags = cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_FIX_PRINCIPAL_POINT | cv::CALIB_ZERO_TANGENT_DIST;
		
		double reprojectionError = cv::aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs, cv::noArray(), cv::noArray(), calibrationFlags);
		intrinsics[cameraNumber] = new Intrinsics(cameraMatrix, distortionCoeffs, reprojectionError);

		BOOST_LOG_TRIVIAL(warning) << "Found intrincs for camera " << cameraNumber;
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

	int originCamera = capturedCameras[0];
	cv::Mat originCameraMatrix = intrinsics[originCamera]->getCameraMatrix();
	cv::Mat originDistortionCoefficients = intrinsics[originCamera]->getDistortionCoefficients();

	bool foundPose = false;
	std::vector<int> charucoIds;
	std::vector<cv::Point2f> charucoCorners;
	cv::Mat frame = sceneController->getFrame(scene, captureType, originCamera, capturedFrames - 1);

	if (detectCharucoCorners(frame, 4, originCameraMatrix, originDistortionCoefficients, charucoIds, charucoCorners))
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
		int totalSamples = 0;		

		BOOST_LOG_TRIVIAL(warning) << "Calculating extrinsics pair " << cameraLeft << "->" << cameraRight;

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

			std::vector<int> finalIds;
			std::vector<cv::Point2f> finalCornersRight;
			std::vector<cv::Point2f> finalCornersLeft;
			std::vector<cv::Point3f> finalObjects;			
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

						finalIds.push_back(idLeft);
						finalObjects.push_back(idPosition);
						finalCornersLeft.push_back(leftPosition);
						finalCornersRight.push_back(rightPosition);
					}
				}
			}

			if (finalObjects.size() >= 4) 
			{
				allObjects.push_back(finalObjects);
				allCornersLeft.push_back(finalCornersLeft);
				allCornersRight.push_back(finalCornersRight);
				totalSamples += (int)finalObjects.size();

				if (outputDetections)
				{
					cv::Mat finalFrameLeft = frameLeft.clone();
					cv::Mat finalFrameRight = frameRight.clone();
					cv::aruco::drawDetectedCornersCharuco(finalFrameLeft, finalCornersLeft, finalIds, cv::Scalar(255, 255, 255));
					cv::aruco::drawDetectedCornersCharuco(finalFrameRight, finalCornersRight, finalIds, cv::Scalar(255, 255, 255));
					sceneController->saveDetections(finalFrameLeft, scene, captureType, cameraLeft, frameNumber, CalibrationType::EXTRINSICS);
					sceneController->saveDetections(finalFrameRight, scene, captureType, cameraRight, frameNumber, CalibrationType::EXTRINSICS);
				}
			}
		}

		BOOST_LOG_TRIVIAL(warning) << "Samples extrinsics pair " << cameraLeft << "->" << cameraRight << ": " << totalSamples;

		cv::Mat translationVector;
		cv::Mat rotationVector;
		cv::Mat essentialMatrix;
		cv::Mat fundamentalMatrix;
		double reprojectionError = cv::stereoCalibrate(allObjects, allCornersLeft, allCornersRight, cameraMatrixLeft, distortionCoefficientsLeft, cameraMatrixRight, distortionCoefficientsRight, cameraSize, rotationVector, translationVector, essentialMatrix, fundamentalMatrix);
		cv::Rodrigues(rotationVector, rotationVector);

		BOOST_LOG_TRIVIAL(warning) << "Found extrinsics pair " << cameraLeft << "->" << cameraRight;

		extrinsics[cameraRight] = new Extrinsics(translationVector, rotationVector, reprojectionError);
	}

	for (int cameraIndex = 1; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int cameraLeft = capturedCameras[cameraIndex - 1];
		int cameraRight = capturedCameras[cameraIndex];

		cv::Mat composedRotationVector;
		cv::Mat composedTranslationVector;
		cv::Mat rotationVectorLeft = extrinsics[cameraLeft]->getRotationVector();
		cv::Mat translationVectorLeft = extrinsics[cameraLeft]->getTranslationVector();
		cv::Mat rotationVectorRight = extrinsics[cameraRight]->getRotationVector();
		cv::Mat translationVectorRight = extrinsics[cameraRight]->getTranslationVector();	

		cv::composeRT(rotationVectorLeft, translationVectorLeft, rotationVectorRight, translationVectorRight, composedRotationVector, composedTranslationVector);
		extrinsics[cameraRight] = new Extrinsics(composedTranslationVector, composedRotationVector, extrinsics[cameraRight]->getReprojectionError());
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
		BOOST_LOG_TRIVIAL(warning) << "Processing cameras in frame " << frameNumber;

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
					std::list<cv::Point3d> composedPoses;

					for (int cornerIndex = 0; cornerIndex < board->chessboardCorners.size(); cornerIndex++)
					{
						cv::Mat cornerModel = cv::Mat(board->chessboardCorners[cornerIndex]);
						cornerModel.convertTo(cornerModel, CV_64F);

						cv::Mat rotationMatrix;
						cv::Rodrigues(rotationVector, rotationMatrix);
						cv::Mat transformedPointCamera = cv::Mat(rotationMatrix * cornerModel + translationVector);

						cv::Mat translationVectorWorld = extrinsics[cameraNumber]->getTranslationVector();
						cv::Mat rotationVectorWorld = extrinsics[cameraNumber]->getRotationVector();
						cv::Mat rotationMatrixWorld;
						cv::Rodrigues(rotationVectorWorld, rotationMatrixWorld);

						cv::Point3d transformedPointWorld = cv::Mat(rotationMatrixWorld.t() * (transformedPointCamera - translationVectorWorld));
						composedPoses.push_back(transformedPointWorld);
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
	std::vector<std::vector<cv::Point2f>> arucoRejections;
	cv::aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params, arucoRejections, cameraMatrix, distortionCoefficients);
	cv::aruco::refineDetectedMarkers(inputImage, board, arucoCorners, arucoIds, arucoRejections, cameraMatrix, distortionCoefficients);

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