#include "Renderer3D.h"

Renderer3D::Renderer3D(ConfigController* configController)
{
	this->totalSquares = configController->getTotalSquares();
	this->squareLength = configController->getSquareLength();
	this->guiFps = configController->getGuiFps();
}

void Renderer3D::render(Video3D* result)
{
	/*viz::Viz3d visualizer = viz::Viz3d("3DPose");
	visualizer.setBackgroundColor(viz::Color(25.0f, 25.0f, 25.0f), viz::Color(50.0f, 50.0f, 50.0f));

	for (int cameraNumber : result->getCameras())
	{
		Mat cameraImage = result->getFrustumImages()[cameraNumber];
		Extrinsics* cameraExtrinsics = result->getExtrinsics()[cameraNumber];
		Intrinsics* cameraIntrinsics = result->getIntrinsics()[cameraNumber];
		Mat cameraMatrix = cameraIntrinsics->getCameraMatrix();
		Matx33d convertedMatrix = Matx33d((double*)cameraMatrix.clone().ptr());
		
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

	int halfPlaneLength = (totalSquares / 2) * squareLength;
	for (int row = 0; row < totalSquares; row++)
	{
		for (int col = 0; col < totalSquares; col++)
		{
			int id = row * totalSquares + col;

			viz::WLine squareLineTop = viz::WLine(Point3d(col * squareLength - halfPlaneLength, row * squareLength - halfPlaneLength, 0), Point3d(col * squareLength + squareLength - halfPlaneLength, row * squareLength - halfPlaneLength, 0), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("top-" + to_string(id), squareLineTop);

			viz::WLine squareLineLeft = viz::WLine(Point3d(col * squareLength - halfPlaneLength, row * squareLength - halfPlaneLength, 0), Point3d(col * squareLength - halfPlaneLength, row * squareLength + squareLength - halfPlaneLength, 0), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("left-" + to_string(id), squareLineLeft);

			viz::WLine squareLineBottom = viz::WLine(Point3d(col * squareLength - halfPlaneLength, (row + 1) * squareLength - halfPlaneLength, 0), Point3d(col * squareLength + squareLength - halfPlaneLength, (row + 1) * squareLength - halfPlaneLength, 0), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("bottom-" + to_string(id), squareLineBottom);

			viz::WLine squareLineRight = viz::WLine(Point3d((col + 1) * squareLength - halfPlaneLength, row * squareLength - halfPlaneLength, 0), Point3d((col + 1) * squareLength - halfPlaneLength, row * squareLength + squareLength - halfPlaneLength, 0), viz::Color(100.0f, 100.0f, 100.0f));
			visualizer.showWidget("right-" + to_string(id), squareLineRight);
		}
	}

	viz::WLine xAxis = viz::WLine(Point3d(0, 0, 0), Point3d(squareLength, 0, 0), viz::Color(59.0f, 85.0f, 237.0f));
	viz::WText3D xAxisText = viz::WText3D("X", Point3d(squareLength + 500, 0, 0), 400.0, true);
	visualizer.showWidget("axis-x", xAxis);
	visualizer.showWidget("axis-x-text", xAxisText);
	visualizer.setRenderingProperty("axis-x", viz::LINE_WIDTH, 2.0);

	viz::WLine yAxis = viz::WLine(Point3d(0, 0, 0), Point3d(0, squareLength, 0), viz::Color(106.0f, 174.0f, 60.0f));
	viz::WText3D yAxisText = viz::WText3D("Y", Point3d(0, squareLength + 500, 0), 400.0, true);
	visualizer.showWidget("axis-y", yAxis);
	visualizer.showWidget("axis-y-text", yAxisText);
	visualizer.setRenderingProperty("axis-y", viz::LINE_WIDTH, 2.0);

	viz::WLine zAxis = viz::WLine(Point3d(0, 0, 0), Point3d(0, 0, squareLength), viz::Color(155.0f, 99.0f, 32.0f));
	viz::WText3D zAxisText = viz::WText3D("Z", Point3d(0, 0, squareLength + 500), 400.0, true);
	visualizer.showWidget("axis-z", zAxis);
	visualizer.showWidget("axis-z-text", zAxisText);
	visualizer.setRenderingProperty("axis-z", viz::LINE_WIDTH, 2.0);

	visualizer.resetCamera();

	int frameNumber = 0;
	vector<map<int, Mat>> posesVideo = result->getPoses();

	visualizer.spinOnce(1, true);
	visualizer
	while (!visualizer.wasStopped())
	{
		printf("Priting frame %d\n", frameNumber);

		if (frameNumber == posesVideo.size())
		{
			frameNumber = 0;
		}

		for (pair<int, Mat> cameraPoses : posesVideo[frameNumber])
		{
			int cameraNumber = cameraPoses.first;
			Mat poses = cameraPoses.second;				
			viz::WCloud cameraCloud = viz::WCloud(poses);
			cameraCloud.setRenderingProperty(cv::viz::POINT_SIZE, 5);
			visualizer.showWidget("cloud-" + to_string(cameraNumber), cameraCloud);
		}
			
		int milisecondsToSleep = (int)(1.0 / guiFps * 1000);
		this_thread::sleep_for(chrono::milliseconds(milisecondsToSleep));

		for (pair<int, Mat> cameraPoses : posesVideo[frameNumber])
		{
			int cameraNumber = cameraPoses.first;
			visualizer.removeWidget("cloud-" + to_string(cameraNumber));
		}

		frameNumber++;		
	}

	delete result;*/
}