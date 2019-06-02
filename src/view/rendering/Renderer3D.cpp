#include "Renderer3D.h"

Renderer3D::Renderer3D(int totalSquares, int squareLength)
{
	this->totalSquares = totalSquares;
	this->squareLength = squareLength;
}

void Renderer3D::render(vector<int> cameras, map<int, Intrinsics*> intrinsics, map<int, Extrinsics*> extrinsics, map<int, Mat> frustumImage)
{
	viz::Viz3d visualizer = viz::Viz3d("3DPose");
	visualizer.setBackgroundColor(viz::Color(25.0f, 25.0f, 25.0f), viz::Color(50.0f, 50.0f, 50.0f));

	for (int cameraNumber : cameras)
	{
		Mat cameraImage = frustumImage[cameraNumber];
		Extrinsics* cameraExtrinsics = extrinsics[cameraNumber];
		Intrinsics* cameraIntrinsics = intrinsics[cameraNumber];
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