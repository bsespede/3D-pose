#include "CameraRenderer.h"

CameraRenderer::CameraRenderer(FileController* fileController)
{
	this->fileController = fileController;
	this->shouldUpdateProportions = true;
	this->barHeight = -1;
	this->cameraWidth = -1;
	this->cameraHeight = -1;
	this->rows = -1;
	this->cols = -1;
}

void CameraRenderer::calculateProportions(float prop, int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int camerasNumber, int barHeight)
{
	int curWidth = (int)(cameraWidth * prop) + 2;
	int curHeight = (int)(cameraHeight * prop) + barHeight + 4;

	int numberWidth = (int)((float)maxWidth / (curWidth + 1));
	int numberHeight = (int)((float)maxHeight / (curHeight + 1));

	int maxCameras = numberWidth * numberHeight;

	if (maxCameras < camerasNumber)
	{
		calculateProportions(prop - 0.01, cameraWidth, cameraHeight, maxWidth, maxHeight, camerasNumber, barHeight);
	}
	else
	{
		this->barHeight = barHeight;
		this->cameraWidth = prop * cameraWidth;
		this->cameraHeight = prop * cameraHeight;
		this->rows = numberHeight;
		this->cols = numberWidth;
	}
}

void CameraRenderer::render(FramesPacket* framesPacket)
{
	int configCameraWidth = fileController->getCameraWidth();
	int configCameraHeight = fileController->getCameraHeight();
	int configMaxWidth = fileController->getMaxWidth();
	int configMaxHeight = fileController->getMaxHeight();
	int configBarHeight = fileController->getBarHeight();
	int configCamerasNumber = framesPacket->getFrames().size();

	calculateProportions(1.0, configCameraWidth, configCameraHeight, configMaxWidth, configMaxHeight, configCamerasNumber, configBarHeight);

	int totalWidth = cols * (cameraWidth + 2) + 1;
	int totalHeight = rows * (cameraHeight + barHeight + 4) + 1;

	Mat mergedImage = Mat(totalHeight, totalWidth, CV_8UC1, Scalar(0));

	for (int u = 0; u < cols; u++)
	{
		for (int v = 0; v < rows; v++)
		{
			int startHorizontal = u * (cameraWidth + 2);
			int startVertical = v * (cameraHeight + barHeight + 4);

			Point2i startLight = Point2i(startHorizontal + 1, startVertical + 1);
			Point2i endLight = Point2i(startLight.x + cameraWidth, startLight.y + barHeight);
			rectangle(mergedImage, startLight, endLight, Scalar(115), FILLED);

			Point2i startMid = Point2i(startLight.x + 1, startLight.y + 1);
			Point2i endMid = Point2i(endLight.x - 1, endLight.y - 1);
			rectangle(mergedImage, startMid, endMid, Scalar(75), FILLED);

			Point2i startDark = Point2i(startLight.x, endLight.y + 2);
			Point2i endDark = Point2i(startDark.x + cameraWidth, startDark.y + cameraHeight);
			rectangle(mergedImage, startDark, endDark, Scalar(25), FILLED);
		}
	}

	for (pair<int, Mat> pair : framesPacket->getFrames())
	{
		int u = pair.first % cols;
		int v = pair.first / cols;

		int startHorizontal = u * (cameraWidth + 2);
		int startVertical = v * (cameraHeight + barHeight + 4);
		Point2i startImage = Point2i(startHorizontal + 1, startVertical + 1 + barHeight + 2);

		Mat resizedImage;
		resize(pair.second, resizedImage, Size(cameraWidth, cameraHeight));
		resizedImage.copyTo(mergedImage(Rect(startImage.x, startImage.y, cameraWidth, cameraHeight)));

		Point2i startText = Point2i(startHorizontal + 5, startVertical + 15);
		Point2i startTextShadow = Point2i(startHorizontal + 6, startVertical + 16);		
		putText(mergedImage, "Camera #" + to_string(pair.first), startTextShadow, FONT_HERSHEY_DUPLEX, 0.4, Scalar(0));
		putText(mergedImage, "Camera #" + to_string(pair.first), startText, FONT_HERSHEY_DUPLEX, 0.4, Scalar(255));
	}

	imshow("Pose3D Cameras", mergedImage);
	waitKey(1);
}