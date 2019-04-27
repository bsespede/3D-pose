#include "CameraRenderer.h"

CameraRenderer::CameraRenderer(int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int camerasNumber, int barHeight)
{
	this->barHeight = barHeight;
	calculateSizeProportion(1.0, cameraWidth, cameraHeight, maxWidth, maxHeight, camerasNumber, barHeight);
}

void CameraRenderer::calculateSizeProportion(float prop, int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int camerasNumber, int barHeight)
{
	int curWidth = (int)(cameraWidth * prop) + 2;
	int curHeight = (int)(cameraHeight * prop) + barHeight + 4;

	int numberWidth = (int)((float)maxWidth / (curWidth + 1));
	int numberHeight = (int)((float)maxHeight / (curHeight + 1));

	int maxCameras = numberWidth * numberHeight;

	if (maxCameras < camerasNumber)
	{
		calculateSizeProportion(prop - 0.01, cameraWidth, cameraHeight, maxWidth, maxHeight, camerasNumber, barHeight);
	}
	else
	{
		this->cameraWidth = prop * cameraWidth;
		this->cameraHeight = prop * cameraHeight;
		this->rows = numberHeight;
		this->cols = numberWidth;
	}
}

void CameraRenderer::render(FramesPacket* framesPacket)
{
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
		Point2i endImage = Point2i(startImage.x + cameraWidth, startImage.y + cameraHeight);

		Mat resizedImage;
		resize(pair.second, resizedImage, Size(cameraWidth, cameraHeight));

		for (int camImageX = startImage.x; camImageX < endImage.x; camImageX++)
		{
			for (int camImageY = startImage.y; camImageY < endImage.y; camImageY++)
			{
				mergedImage.at<uchar>(camImageY, camImageX) = resizedImage.at<uchar>(camImageY - startImage.y, camImageX - startImage.x);
			}
		}

		Point2i startText = Point2i(startHorizontal + 5, startVertical + 15);
		Point2i startTextShadow = Point2i(startHorizontal + 6, startVertical + 16);

		int cameraNumber = u + v * cols;
		putText(mergedImage, "Camera #" + to_string(pair.first), startTextShadow, FONT_HERSHEY_DUPLEX, 0.4, Scalar(0));
		putText(mergedImage, "Camera #" + to_string(pair.first), startText, FONT_HERSHEY_DUPLEX, 0.4, Scalar(255));
	}

	imshow("Pose3D Cameras", mergedImage);
	waitKey(1);
}