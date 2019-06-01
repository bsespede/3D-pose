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
			
			if (detectCharucoCorners(frame, result, 4, charucoIds, charucoCorners))
			{
				allCharucoIds.push_back(charucoIds);
				allCharucoCorners.push_back(charucoCorners);		
			}			
		}

		Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
		Mat distortionCoeffs = Mat::zeros(1, 5, CV_64F);
		int calibrationFlags = CALIB_FIX_ASPECT_RATIO;
		
		double reprojectionError = aruco::calibrateCameraCharuco(allCharucoCorners, allCharucoIds, board, frameSize, cameraMatrix, distortionCoeffs, noArray(), noArray(), calibrationFlags);
		calibrationResults[cameraNumber] = new Intrinsics(cameraMatrix, distortionCoeffs, reprojectionError);		
	}

	fileController->saveIntrinsics(calibrationResults);
	return true;
}

bool CalibrationController::calculateExtrinsics(Scene scene, Operation operation)
{
	map<int, Extrinsics*> calibrationResults;
	int capturedFrames = fileController->getCapturedFrames(scene, operation);
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, operation);

	#pragma omp parallel for
	for (int cameraIndex = 0; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int cameraNumber = capturedCameras[cameraIndex];

		vector<int> charucoIds;
		vector<Point2f> charucoCorners;
		Mat frame = fileController->getCapturedFrame(scene, operation, cameraNumber, capturedFrames - 1);
		Mat frameResult = frame.clone();

		Intrinsics * intrinsics = fileController->getIntrinsics(cameraNumber);
		Mat cameraMatrix = intrinsics->getCameraMatrix();
		Mat distortionCoefficients = intrinsics->getDistortionCoefficients();

		if (detectCharucoCorners(frame, frameResult, 1, cameraMatrix, distortionCoefficients, charucoIds, charucoCorners))
		{
			BOOST_LOG_TRIVIAL(warning) << "Found board in the floor of camera " << cameraNumber;

			Mat originRotationVector;
			Mat originTranslationVector;
			aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraMatrix, distortionCoefficients, originRotationVector, originTranslationVector);

			aruco::drawAxis(frameResult, cameraMatrix, distortionCoefficients, originRotationVector, originTranslationVector, 10.0);
			calibrationResults[cameraNumber] = new Extrinsics(originTranslationVector, originRotationVector, intrinsics->getReprojectionError());
		}

		fileController->saveCalibrationDetections(frameResult, scene, operation, cameraNumber, capturedFrames - 1);
	}

	int originCamera = 0;
	if (!calibrationResults.count(originCamera))
	{
		Mat originTranslationVector = Mat::zeros(3, 1, CV_64F);
		Mat originRotationVector = Mat::zeros(3, 1, CV_64F);
		calibrationResults[originCamera] = new Extrinsics(originTranslationVector, originRotationVector, 0.0);
	}

	#pragma omp parallel for
	for (int cameraIndex = 1; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int cameraLeft = capturedCameras[cameraIndex - 1];
		int cameraRight = capturedCameras[cameraIndex];

		BOOST_LOG_TRIVIAL(warning) << "Calibrating for pair " << cameraLeft << "->" << cameraRight;

		if (calibrationResults.count(cameraRight))
		{
			break;
		}

		int totalSamples = 0;		
		Size cameraSize;
		
		Intrinsics* intrinsicsLeft = fileController->getIntrinsics(cameraLeft);
		Mat cameraMatrixLeft = intrinsicsLeft->getCameraMatrix();
		Mat distortionCoefficientsLeft = intrinsicsLeft->getDistortionCoefficients();

		Intrinsics* intrinsicsRight = fileController->getIntrinsics(cameraRight);
		Mat cameraMatrixRight = intrinsicsRight->getCameraMatrix();
		Mat distortionCoefficientsRight = intrinsicsRight->getDistortionCoefficients();		

		vector<vector<Point3f>> allObjects;
		vector<vector<Point2f>> allCornersLeft;
		vector<vector<Point2f>> allCornersRight;		
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber += 5)
		{
			vector<int> idsLeft;
			vector<Point2f> cornersLeft;
			Mat frameLeft = fileController->getCapturedFrame(scene, operation, cameraLeft, frameNumber);
			Mat frameLeftResult = frameLeft.clone();
			cameraSize = frameLeft.size();

			if (!detectCharucoCorners(frameLeft, frameLeftResult, 4, cameraMatrixLeft, distortionCoefficientsLeft, idsLeft, cornersLeft))
			{
				continue;
			}

			vector<int> idsRight;
			vector<Point2f> cornersRight;
			Mat frameRight = fileController->getCapturedFrame(scene, operation, cameraRight, frameNumber);
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
				totalSamples += finalObjects.size();
			}
		}

		BOOST_LOG_TRIVIAL(warning) << "Samples for pair " << cameraLeft << "->" << cameraRight << ": " << totalSamples;

		Mat translationVector;
		Mat rotationVector;
		Mat essentialMatrix;
		Mat fundamentalMatrix;
		int flags = CALIB_FIX_ASPECT_RATIO;

		double reprojectionError = cv::stereoCalibrate(allObjects, allCornersLeft, allCornersRight, cameraMatrixLeft, distortionCoefficientsLeft, cameraMatrixRight, distortionCoefficientsRight, cameraSize, rotationVector, translationVector, essentialMatrix, fundamentalMatrix);
		cv::Rodrigues(rotationVector, rotationVector);

		calibrationResults[cameraRight] = new Extrinsics(translationVector, rotationVector, reprojectionError);
		BOOST_LOG_TRIVIAL(warning) << "Finished calibrating for pair " << cameraLeft << "->" << cameraRight;
	}

	for (int cameraNumber = 1; cameraNumber < (int)calibrationResults.size(); cameraNumber++)
	{
		Mat composedTranslationVector;
		Mat composedRotationVector;
		Mat currentTranslationVector = calibrationResults[cameraNumber]->getTranslationVector();
		Mat currentRotationVector = calibrationResults[cameraNumber]->getRotationVector();
		Mat previousTranslationVector = calibrationResults[cameraNumber - 1]->getTranslationVector();
		Mat previousRotationVector = calibrationResults[cameraNumber - 1]->getRotationVector();

		cv::composeRT(previousRotationVector, previousTranslationVector, currentRotationVector, currentTranslationVector, composedRotationVector, composedTranslationVector);
		calibrationResults[cameraNumber] = new Extrinsics(composedTranslationVector, composedRotationVector, calibrationResults[cameraNumber]->getReprojectionError());
	}

	fileController->saveExtrinsics(scene, calibrationResults);
	renderCalibration(scene);
	return true;
}

bool CalibrationController::detectCharucoCorners(Mat& inputImage, Mat& outputImage, int minCharucoCorners, vector<int>& charucoIds, vector<Point2f>& charucoCorners)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners;	
	aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params);

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
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, Operation::EXTRINSICS);

	for (int cameraNumber : capturedCameras)
	{
		Extrinsics* cameraExtrinsics = fileController->getExtrinsics(scene, cameraNumber);
		Intrinsics* cameraIntrinsics = fileController->getIntrinsics(cameraNumber);
		Mat cameraImage = fileController->getCapturedFrame(scene, Operation::EXTRINSICS, cameraNumber, 0);

		Mat cameraMatrix = cameraIntrinsics->getCameraMatrix();
		Matx33d convertedMatrix = Matx33d((double*)cameraMatrix.clone().ptr());
		viz::WCameraPosition cameraWidget = viz::WCameraPosition(convertedMatrix, 500.0);
		visualizer.showWidget("camera-" + to_string(cameraNumber), cameraWidget);

		Mat rotationMatrix;
		Mat rotationVector = cameraExtrinsics->getRotationVector();
		Rodrigues(rotationVector, rotationMatrix);
		Mat translationVector = cameraExtrinsics->getTranslationVector();
		Affine3d cameraPose = Affine3d(rotationMatrix, translationVector);
		visualizer.setWidgetPose("camera-" + to_string(cameraNumber), cameraPose);
		
		viz::WText3D cameraNumberWidget = viz::WText3D("   " +to_string(cameraNumber), Point3d(translationVector), 400.0, true);
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

			viz::WLine squareLineTop = viz::WLine(Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3f(col * squareLength + squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("top-" + to_string(id), squareLineTop);

			viz::WLine squareLineLeft = viz::WLine(Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength + squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("left-" + to_string(id), squareLineLeft);

			viz::WLine squareLineBottom = viz::WLine(Point3f(col * squareLength - halfPlaneLength, 0, (row + 1) * squareLength - halfPlaneLength), Point3f(col * squareLength + squareLength - halfPlaneLength, 0, (row + 1) * squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("bottom-" + to_string(id), squareLineBottom);
			
			viz::WLine squareLineRight = viz::WLine(Point3f((col + 1) * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3f((col + 1) * squareLength - halfPlaneLength, 0, row * squareLength + squareLength - halfPlaneLength), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("right-" + to_string(id), squareLineRight);
		}
	}

	viz::WLine xAxis = viz::WLine(Point3f(-halfPlaneLength, 0, -halfPlaneLength), Point3f(-halfPlaneLength + squareLength, 0, -halfPlaneLength), viz::Color::red());
	visualizer.showWidget("x", xAxis);
	visualizer.setRenderingProperty("x", viz::LINE_WIDTH, 3.0);
	
	viz::WLine yAxis = viz::WLine(Point3f(-halfPlaneLength, 0, -halfPlaneLength), Point3f(-halfPlaneLength, squareLength, -halfPlaneLength), viz::Color::green());
	visualizer.showWidget("y", yAxis);
	visualizer.setRenderingProperty("y", viz::LINE_WIDTH, 3.0);

	viz::WLine zAxis = viz::WLine(Point3f(-halfPlaneLength, 0, -halfPlaneLength), Point3f(-halfPlaneLength, 0, -halfPlaneLength + squareLength), viz::Color::blue());
	visualizer.showWidget("z", zAxis);	
	visualizer.setRenderingProperty("z", viz::LINE_WIDTH, 3.0);

	visualizer.resetCamera();

	while (!visualizer.wasStopped())
	{
		visualizer.spinOnce(1, true);
	}
}