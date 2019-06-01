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
	int capturedFrames = fileController->getCapturedFrames(scene, operation);
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, operation);
	
	/*int originCamera = 0;	
	Mat originFrame = fileController->getCapturedFrame(scene, operation, originCamera, capturedFrames - 1);
	Mat originFrameResult = originFrame.clone();

	Intrinsics* intrinsics = fileController->getIntrinsics(originCamera);
	Mat originCameraMatrix = intrinsics->getCameraMatrix();
	Mat originDistortionCoefficients = intrinsics->getDistortionCoefficients();
	Mat originRotationVector;
	Mat originTranslationVector;

	if (detectCharucoPose(originFrame, originFrameResult, originCameraMatrix, originDistortionCoefficients, originRotationVector, originTranslationVector))
	{
		calibrationResults[originCamera] = new Extrinsics(originTranslationVector, originRotationVector, intrinsics->getReprojectionError());
		fileController->saveCalibrationDetections(originFrameResult, scene, operation, originCamera, capturedFrames - 1);
	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "Could not find board in reference frame";
	}*/

	int originCamera = 0;
	Mat originTranslationVector = Mat::zeros(3, 1, CV_64F);
	Mat originRotationVector = Mat::zeros(3, 1, CV_64F);
	originRotationVector.at<double>(1, 0) = 1.0;
	calibrationResults[originCamera] = new Extrinsics(originTranslationVector, originRotationVector, 0.0);

	#pragma omp parallel for
	for (int cameraIndex = 1; cameraIndex < (int)capturedCameras.size(); cameraIndex++)
	{
		int totalSamples = 0;
		int cameraLeft = capturedCameras[cameraIndex - 1];
		int cameraRight = capturedCameras[cameraIndex];
		Size cameraSize;

		BOOST_LOG_TRIVIAL(warning) << "Calibrating for pair " << cameraLeft << "->" << cameraRight;

		vector<vector<Point3f>> allObjects;
		vector<vector<Point2f>> allCornersLeft;
		vector<vector<Point2f>> allCornersRight;		
		for (int frameNumber = 0; frameNumber < capturedFrames; frameNumber++)
		{
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

		Intrinsics* leftIntrinsics = fileController->getIntrinsics(cameraLeft);
		Intrinsics* rightIntrinsics = fileController->getIntrinsics(cameraRight);
		Mat translationVector;
		Mat rotationVector;
		Mat essentialMatrix;
		Mat fundamentalMatrix;

		double reprojectionError = cv::stereoCalibrate(allObjects, allCornersLeft, allCornersRight, leftIntrinsics->getCameraMatrix(), leftIntrinsics->getDistortionCoefficients(), rightIntrinsics->getCameraMatrix(), rightIntrinsics->getDistortionCoefficients(), cameraSize, rotationVector, translationVector, essentialMatrix, fundamentalMatrix);
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

bool CalibrationController::detectCharucoCorners(Mat& inputImage, Mat& outputImage, vector<int>& charucoIds, vector<Point2f>& charucoCorners)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners;
	vector<vector<Point2f>> arucoRejections;
	aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params);

	if (arucoIds.size() > 0)
	{
		aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds);

		if (charucoIds.size() > 4)
		{
			aruco::drawDetectedMarkers(outputImage, arucoCorners);
			aruco::drawDetectedCornersCharuco(outputImage, charucoCorners, charucoIds, Scalar(255, 255, 255));
			return true;
		}
	}
	
	return false;
}

bool CalibrationController::detectCharucoPose(Mat& inputImage, Mat& outputImage, Mat& cameraMatrix, Mat& distortionCoefficients, Mat& rotationVector, Mat& translationVector)
{
	Ptr<aruco::DetectorParameters> params = aruco::DetectorParameters::create();
	params->cornerRefinementMethod = aruco::CORNER_REFINE_NONE;

	vector<int> arucoIds;
	vector<vector<Point2f>> arucoCorners;
	vector<vector<Point2f>> arucoRejections;
	aruco::detectMarkers(inputImage, dictionary, arucoCorners, arucoIds, params, arucoRejections, cameraMatrix, distortionCoefficients);

	if (arucoIds.size() > 0)
	{
		vector<int> charucoIds;
		vector<Point2f> charucoCorners;
		aruco::interpolateCornersCharuco(arucoCorners, arucoIds, inputImage, board, charucoCorners, charucoIds, cameraMatrix, distortionCoefficients);

		if (charucoIds.size() > 4)
		{
			aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraMatrix, distortionCoefficients, rotationVector, translationVector);			
			aruco::drawDetectedMarkers(outputImage, arucoCorners);
			aruco::drawDetectedCornersCharuco(outputImage, charucoCorners, charucoIds, Scalar(255, 255, 255));
			aruco::drawAxis(outputImage, cameraMatrix, distortionCoefficients, rotationVector, translationVector, 10.0);			
			return true;
		}
	}

	return false;
}

void CalibrationController::renderCalibration(Scene scene)
{
	viz::Viz3d visualizer = viz::Viz3d("3DPose");
	visualizer.setBackgroundColor(viz::Color(0.15f, 0.15f, 0.15f), viz::Color(0.25f, 0.25f, 0.25f));
	vector<int> capturedCameras = fileController->getCapturedCameras(scene, Operation::EXTRINSICS);

	for (int cameraNumber : capturedCameras)
	{
		Extrinsics* cameraExtrinsics = fileController->getExtrinsics(scene, cameraNumber);
		Intrinsics* cameraIntrinsics = fileController->getIntrinsics(cameraNumber);
		Mat cameraImage = fileController->getCapturedFrame(scene, Operation::EXTRINSICS, cameraNumber, 0);

		Mat cameraMatrix = cameraIntrinsics->getCameraMatrix();
		Matx33d convertedMatrix = Matx33d((double*)cameraMatrix.clone().ptr());
		viz::WCameraPosition cameraWidget = viz::WCameraPosition(convertedMatrix, cameraImage);
		visualizer.showWidget("camera-" + to_string(cameraNumber), cameraWidget);

		Mat rotationMatrix;
		Mat rotationVector = cameraExtrinsics->getRotationVector();
		Rodrigues(rotationVector, rotationMatrix);
		Mat translationVector = cameraExtrinsics->getTranslationVector();
		Affine3d cameraPose = Affine3d(rotationMatrix, translationVector);

		visualizer.setWidgetPose("camera-" + to_string(cameraNumber), cameraPose);
	}

	int totalSquares = 20;
	int squareLength = 1000;
	int halfPlaneLength = (totalSquares / 2) * squareLength;

	for (int row = 0; row < totalSquares; row++)
	{
		for (int col = 0; col < totalSquares; col++)
		{
			int id = row * col + col;

			viz::WLine squareLineTop = viz::WLine(Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3f(col * squareLength + squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength));
			squareLineTop.setRenderingProperty(viz::LINE_WIDTH, 1.0);
			visualizer.showWidget("top-" + to_string(id), squareLineTop);

			viz::WLine squareLineLeft = viz::WLine(Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength + squareLength - halfPlaneLength));
			squareLineLeft.setRenderingProperty(viz::LINE_WIDTH, 1.0);
			visualizer.showWidget("left-" + to_string(id), squareLineLeft);

			if (col == totalSquares - 1)
			{
				viz::WLine squareLineBottom = viz::WLine(Point3f(col * squareLength + squareLength - halfPlaneLength, 0, row * squareLength - halfPlaneLength), Point3f(col * squareLength + squareLength * 2 - halfPlaneLength, 0, row * squareLength - halfPlaneLength));
				squareLineBottom.setRenderingProperty(viz::LINE_WIDTH, 1.0);
				visualizer.showWidget("bottom-" + to_string(id), squareLineBottom);
			}

			if (row == totalSquares - 1)
			{
				viz::WLine squareLineRight = viz::WLine(Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength + squareLength - halfPlaneLength), Point3f(col * squareLength - halfPlaneLength, 0, row * squareLength + squareLength * 2 - halfPlaneLength));
				squareLineRight.setRenderingProperty(viz::LINE_WIDTH, 1.0);
				visualizer.showWidget("right-" + to_string(id), squareLineRight);
			}
		}
	}

	while (!visualizer.wasStopped())
	{
		visualizer.spinOnce(1, true);
	}
}