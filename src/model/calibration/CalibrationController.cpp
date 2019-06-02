#include "CalibrationController.h"

CalibrationController::CalibrationController(ConfigController* configController, SceneController* sceneController)
{
	int charucoCols = configController->getCharucoCols();
	int charucoRows = configController->getCharucoRows();
	float charucoSquareLength = configController->getCharucoSquareLength();
	float charucoMarkerLength = configController->getCharucoMarkerLength();

	this->configController = configController;
	this->sceneController = sceneController;
	this->dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
	this->board = aruco::CharucoBoard::create(charucoCols, charucoRows, charucoSquareLength, charucoMarkerLength, dictionary);
}

void CalibrationController::generateCheckboard()
{
	int charucoWidth = configController->getCharucoWidth();
	int charucoHeight = configController->getCharucoHeight();
	int charucoMargin = configController->getCharucoMargin();

	Mat boardImage;
	board->draw(Size(charucoWidth, charucoHeight), boardImage, charucoMargin, 1);
	imwrite("board.png", boardImage);
}

bool CalibrationController::calibrate(Scene scene, Operation operation)
{
	if (!sceneController->hasCapture(scene, operation))
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
	int capturedFrames = sceneController->getCapturedFrameNumber(scene, operation);
	vector<int> capturedCameras = sceneController->getCapturedCameras(scene, operation);

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
			Mat frame = sceneController->getCapturedFrame(scene, operation, cameraNumber, frameNumber);
			Mat result = frame.clone();
			frameSize = frame.size();
			
			if (detectCharucoCorners(frame, result, 5, charucoIds, charucoCorners))
			{
				allCharucoIds.push_back(charucoIds);
				allCharucoCorners.push_back(charucoCorners);		
			}			
		}

		Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
		Mat distortionCoeffs = Mat::zeros(1, 5, CV_64F);
		int calibrationFlags = CALIB_FIX_ASPECT_RATIO | CALIB_FIX_PRINCIPAL_POINT | CALIB_ZERO_TANGENT_DIST;
		
		double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs, noArray(), noArray(), calibrationFlags);
		calibrationResults[cameraNumber] = new Intrinsics(cameraMatrix, distortionCoeffs, reprojectionError);		
	}

	sceneController->saveIntrinsics(calibrationResults);
	return true;
}

bool CalibrationController::calculateExtrinsics(Scene scene, Operation operation)
{
	renderCalibration(scene);

	int capturedFrames = sceneController->getCapturedFrameNumber(scene, operation);
	vector<int> capturedCameras = sceneController->getCapturedCameras(scene, operation);

	map<int, Extrinsics*> poseResults;
	map<int, Intrinsics*> calibrationResults;

	for (int cameraNumber : capturedCameras)
	{
		calibrationResults[cameraNumber] = sceneController->getIntrinsics(cameraNumber);
	}	

	#pragma omp parallel for
	for (int cameraIndex = 0; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int cameraNumber = capturedCameras[cameraIndex];

		vector<int> charucoIds;
		vector<Point2f> charucoCorners;
		Mat frame = sceneController->getCapturedFrame(scene, operation, cameraNumber, capturedFrames - 1);
		Mat frameResult = frame.clone();

		Intrinsics * intrinsics = calibrationResults[cameraNumber];
		Mat cameraMatrix = intrinsics->getCameraMatrix();
		Mat distortionCoefficients = intrinsics->getDistortionCoefficients();

		if (detectCharucoCorners(frame, frameResult, 1, cameraMatrix, distortionCoefficients, charucoIds, charucoCorners))
		{
			BOOST_LOG_TRIVIAL(warning) << "Found board in the floor of camera " << cameraNumber;

			Mat originRotationVector;
			Mat originTranslationVector;
			aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraMatrix, distortionCoefficients, originRotationVector, originTranslationVector);

			aruco::drawAxis(frameResult, cameraMatrix, distortionCoefficients, originRotationVector, originTranslationVector, 10.0);
			poseResults[cameraNumber] = new Extrinsics(originTranslationVector, originRotationVector, intrinsics->getReprojectionError());
		}

		sceneController->saveCalibrationDetections(frameResult, scene, operation, cameraNumber, capturedFrames - 1);
	}

	int originCamera = 0;
	if (!poseResults.count(originCamera))
	{
		Mat originTranslationVector = Mat::zeros(3, 1, CV_64F);
		Mat originRotationVector = Mat::zeros(3, 1, CV_64F);
		poseResults[originCamera] = new Extrinsics(originTranslationVector, originRotationVector, 0.0);
	}

	#pragma omp parallel for
	for (int cameraIndex = 1; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int cameraLeft = capturedCameras[cameraIndex - 1];
		int cameraRight = capturedCameras[cameraIndex];

		BOOST_LOG_TRIVIAL(warning) << "Calibrating for pair " << cameraLeft << "->" << cameraRight;

		if (poseResults.count(cameraRight))
		{
			break;
		}

		int totalSamples = 0;		
		Size cameraSize;
		
		Intrinsics* intrinsicsLeft = calibrationResults[cameraLeft];
		Mat cameraMatrixLeft = intrinsicsLeft->getCameraMatrix();
		Mat distortionCoefficientsLeft = intrinsicsLeft->getDistortionCoefficients();

		Intrinsics* intrinsicsRight = calibrationResults[cameraRight];
		Mat cameraMatrixRight = intrinsicsRight->getCameraMatrix();
		Mat distortionCoefficientsRight = intrinsicsRight->getDistortionCoefficients();		

		vector<vector<Point3f>> allObjects;
		vector<vector<Point2f>> allCornersLeft;
		vector<vector<Point2f>> allCornersRight;		
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
			vector<int> idsLeft;
			vector<Point2f> cornersLeft;
			Mat frameLeft = sceneController->getCapturedFrame(scene, operation, cameraLeft, frameNumber);
			Mat frameLeftResult = frameLeft.clone();
			cameraSize = frameLeft.size();

			if (!detectCharucoCorners(frameLeft, frameLeftResult, 4, cameraMatrixLeft, distortionCoefficientsLeft, idsLeft, cornersLeft))
			{
				continue;
			}

			vector<int> idsRight;
			vector<Point2f> cornersRight;
			Mat frameRight = sceneController->getCapturedFrame(scene, operation, cameraRight, frameNumber);
			Mat frameRightResult = frameRight.clone();

			if (!detectCharucoCorners(frameRight, frameRightResult, 4, cameraMatrixLeft, distortionCoefficientsRight, idsRight, cornersRight))
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

		calibrationResults[cameraLeft] = new Intrinsics(cameraMatrixLeft, distortionCoefficientsLeft, reprojectionError);
		calibrationResults[cameraRight] = new Intrinsics(cameraMatrixRight, distortionCoefficientsRight, reprojectionError);
		poseResults[cameraRight] = new Extrinsics(translationVector, rotationVector, reprojectionError);

		BOOST_LOG_TRIVIAL(warning) << "Finished calibrating for pair " << cameraLeft << "->" << cameraRight;
	}

	for (int cameraNumber = 1; cameraNumber < (int)poseResults.size(); cameraNumber++)
	{
		Mat composedTranslationVector;
		Mat composedRotationVector;
		Mat currentTranslationVector = poseResults[cameraNumber]->getTranslationVector();
		Mat currentRotationVector = poseResults[cameraNumber]->getRotationVector();
		Mat previousTranslationVector = poseResults[cameraNumber - 1]->getTranslationVector();
		Mat previousRotationVector = poseResults[cameraNumber - 1]->getRotationVector();

		cv::composeRT(previousRotationVector, previousTranslationVector, currentRotationVector, currentTranslationVector, composedRotationVector, composedTranslationVector);
		poseResults[cameraNumber] = new Extrinsics(composedTranslationVector, composedRotationVector, poseResults[cameraNumber]->getReprojectionError());
	}

	sceneController->saveIntrinsics(calibrationResults);
	sceneController->saveExtrinsics(scene, poseResults);
	return true;
}

bool CalibrationController::detectCharucoCorners(Mat& inputImage, Mat& outputImage, int minCharucoCorners, vector<int>& charucoIds, vector<Point2f>& charucoCorners)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners, arucoRejections;	
	aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params);
	aruco::refineDetectedMarkers(inputImage, board, arucoCorners, arucoIds, arucoRejections);

	if (arucoIds.size() > 0)
	{
		aruco::drawDetectedMarkers(outputImage, arucoCorners);
		aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds);

		if (charucoIds.size() >= minCharucoCorners)
		{
			aruco::drawDetectedCornersCharuco(outputImage, charucoCorners, charucoIds, Scalar(255, 255, 255));
			return true;
		}
	}

	return false;
}

bool CalibrationController::detectCharucoCorners(Mat& inputImage, Mat& outputImage, int minCharucoCorners, Mat& cameraMatrix, Mat& distortionCoefficients, vector<int>& charucoIds, vector<Point2f>& charucoCorners)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners, arucoRejections;
	aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params, arucoRejections, cameraMatrix, distortionCoefficients);
	aruco::refineDetectedMarkers(inputImage, board, arucoCorners, arucoIds, arucoRejections, cameraMatrix, distortionCoefficients);

	if (arucoIds.size() > 0)
	{
		aruco::drawDetectedMarkers(outputImage, arucoCorners);
		aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds, cameraMatrix, distortionCoefficients);

		if (charucoIds.size() >= minCharucoCorners)
		{
			aruco::drawDetectedCornersCharuco(outputImage, charucoCorners, charucoIds, Scalar(255, 255, 255));
			return true;
		}
	}
	return false;
}

void CalibrationController::renderCalibration(Scene scene)
{
	viz::Viz3d visualizer = viz::Viz3d("3DPose");
	visualizer.setBackgroundColor(viz::Color(25.0f, 25.0f, 25.0f), viz::Color(50.0f, 50.0f, 50.0f));
	vector<int> capturedCameras = sceneController->getCapturedCameras(scene, Operation::EXTRINSICS);

	for (int cameraNumber : capturedCameras)
	{
		Extrinsics* cameraExtrinsics = sceneController->getExtrinsics(scene, cameraNumber);
		Intrinsics* cameraIntrinsics = sceneController->getIntrinsics(cameraNumber);
		Mat cameraMatrix = cameraIntrinsics->getCameraMatrix();
		Matx33d convertedMatrix = Matx33d((double*)cameraMatrix.clone().ptr());
		Mat cameraImage = sceneController->getCapturedFrame(scene, Operation::EXTRINSICS, cameraNumber, 0);		
		viz::WCameraPosition cameraWidget = viz::WCameraPosition(convertedMatrix, cameraImage, 1000.0);
		visualizer.showWidget("camera-" + to_string(cameraNumber), cameraWidget);

		Mat rotationMatrix;
		Mat rotationVector = cameraExtrinsics->getRotationVector();
		Rodrigues(rotationVector, rotationMatrix);
		Mat translationVector = cameraExtrinsics->getTranslationVector();
		Affine3d cameraPose = Affine3d(rotationMatrix, translationVector);
		visualizer.setWidgetPose("camera-" + to_string(cameraNumber), cameraPose);
		
		viz::WText3D cameraNumberWidget = viz::WText3D(to_string(cameraNumber), Point3d(translationVector), 400.0, true);
		visualizer.showWidget("text-" + to_string(cameraNumber), cameraNumberWidget);		
	}

	int totalSquares = 10;
	int squareLength = 1500;
	int halfPlaneLength = (totalSquares / 2) * squareLength;

	for (int row = 0; row < totalSquares; row++)
	{
		for (int col = 0; col < totalSquares; col++)
		{
			int id = row * totalSquares + col;

			viz::WLine squareLineTop = viz::WLine(Point3d(col * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3d(col * squareLength + squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("top-" + to_string(id), squareLineTop);

			viz::WLine squareLineLeft = viz::WLine(Point3d(col * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3d(col * squareLength - halfPlaneLength, 0, row * squareLength + squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("left-" + to_string(id), squareLineLeft);

			viz::WLine squareLineBottom = viz::WLine(Point3d(col * squareLength - halfPlaneLength, 0, (row + 1) * squareLength - halfPlaneLength), Point3d(col * squareLength + squareLength - halfPlaneLength, 0, (row + 1) * squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("bottom-" + to_string(id), squareLineBottom);
			
			viz::WLine squareLineRight = viz::WLine(Point3d((col + 1) * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3d((col + 1) * squareLength - halfPlaneLength, 0, row * squareLength + squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("right-" + to_string(id), squareLineRight);
		}
	}

	viz::WLine xAxis = viz::WLine(Point3d(-halfPlaneLength, 0, -halfPlaneLength), Point3d(-halfPlaneLength + squareLength, 0, -halfPlaneLength), viz::Color(59.0f, 85.0f, 237.0f));
	viz::WText3D xAxisText = viz::WText3D("X", Point3d(-halfPlaneLength + squareLength + 500, 0, -halfPlaneLength), 400.0, true);
	visualizer.showWidget("axis-x", xAxis);
	visualizer.showWidget("axis-x-text", xAxisText);
	visualizer.setRenderingProperty("axis-x", viz::LINE_WIDTH, 2.0);	
	
	viz::WLine yAxis = viz::WLine(Point3d(-halfPlaneLength, 0, -halfPlaneLength), Point3d(-halfPlaneLength, squareLength, -halfPlaneLength), viz::Color(106.0f, 174.0f, 60.0f));
	viz::WText3D yAxisText = viz::WText3D("Y", Point3d(-halfPlaneLength, squareLength + 500, -halfPlaneLength), 400.0, true);
	visualizer.showWidget("axis-y", yAxis);
	visualizer.showWidget("axis-y-text", yAxisText);
	visualizer.setRenderingProperty("axis-y", viz::LINE_WIDTH, 2.0);

	viz::WLine zAxis = viz::WLine(Point3d(-halfPlaneLength, 0, -halfPlaneLength), Point3d(-halfPlaneLength, 0, -halfPlaneLength + squareLength), viz::Color(155.0f, 99.0f, 32.0f));
	viz::WText3D zAxisText = viz::WText3D("Z", Point3d(-halfPlaneLength, 0, -halfPlaneLength + squareLength + 500), 400.0, true);
	visualizer.showWidget("axis-z", zAxis);	
	visualizer.showWidget("axis-z-text", zAxisText);
	visualizer.setRenderingProperty("axis-z", viz::LINE_WIDTH, 2.0);	

	visualizer.resetCamera();

	while (!visualizer.wasStopped())
	{
		visualizer.spinOnce(1, true);
	}
}