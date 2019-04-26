#include "CameraRenderer.h"

CameraRenderer::CameraRenderer(int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int cameraNumber, int barHeight)
{
	this->barHeight = barHeight;
	this->cameraHeight = cameraHeight;
	this->cameraWidth = cameraWidth;
	calculateSizeProportion(1.0, cameraWidth, cameraHeight, maxWidth, maxHeight, cameraNumber, barHeight);
}

void CameraRenderer::calculateSizeProportion(float prop, int cameraWidth, int cameraHeight, int maxWidth, int maxHeight, int cameraNumber, int barHeight)
{
	int curWidth = cameraWidth * prop + 1;
	int curHeight = cameraHeight * prop + barHeight + 2;

	int numberWidth = (int)((float)maxWidth / (curWidth + 1));
	int numberHeight = (int)((float)maxHeight / (curHeight + 1));

	int maxCameras = numberWidth * numberHeight;

	if (maxCameras < cameraNumber)
	{
		calculateSizeProportion(prop - 0.05, cameraWidth, cameraHeight, maxWidth, maxHeight, cameraNumber, barHeight);
	}
	else
	{
		this->prop = prop;
		this->rows = numberHeight;
		this->cols = numberWidth;
	}
}

void CameraRenderer::render(FramesPacket* framesPacket)
{
	int totalWidth = cols * (prop * cameraWidth + 1) + 1;
	int totalHeight = rows * (prop * cameraHeight + barHeight + 2) + 1;

	Mat mergedImage = Mat(totalHeight, totalWidth, CV_8UC1, Scalar(15));

	for (int u = 0; u < cols; u++)
	{
		for (int v = 0; v < rows; v++)
		{
			int startHorizontal = u * (prop * cameraWidth + 1);
			int startVertical = v * (prop * cameraHeight + barHeight + 2);

			Point2i startLight = Point2i(1 + startHorizontal, 1 + startVertical);
			Point2i endLight = Point2i(startLight.x + prop * cameraWidth, startLight.y + barHeight);
			rectangle(mergedImage, startLight, endLight, Scalar(100), FILLED);

			Point2i startMid = Point2i(startLight.x + 1, startLight.y + 1);
			Point2i endMid = Point2i(endLight.x - 1, endLight.y - 1);
			rectangle(mergedImage, startMid, endMid, Scalar(75), FILLED);

			Point2i startDark = Point2i(startLight.x, endLight.y + 2);
			Point2i endDark = Point2i(startDark.x + prop * cameraWidth, startDark.y + prop * cameraHeight);
			rectangle(mergedImage, startDark, endDark, Scalar(25), FILLED);

			Point2i startText = Point2i(startLight.x + 4, startLight.y + 4);
			Point2i startTextShadow = Point2i(startLight.x + 5, startLight.y + 5);

			int cameraNumber = u + v * cols;
			putText(mergedImage, "Camera #" + to_string(cameraNumber), startTextShadow, FONT_HERSHEY_DUPLEX, 0.4, Scalar(0));
			putText(mergedImage, "Camera #" + to_string(cameraNumber), startText, FONT_HERSHEY_DUPLEX, 0.4, Scalar(255));
		}
	}

	for (pair<int, Mat> pair : framesPacket->getFrames()) {
		int u = pair.first % cols;
		int v = pair.first / rows;

		int startHorizontal = u * (prop * cameraWidth + 1);
		int startVertical = v * (prop * cameraHeight + barHeight + 2);

		Point2i startImage = Point2i(startHorizontal + 1, startVertical + barHeight + 2);
		Point2i endImage = Point2i(startImage.x + prop * cameraWidth, startImage.y + prop * cameraHeight);

		Mat resizedImage;
		resize(pair.second, resizedImage, Size(prop * cameraWidth, prop * cameraHeight));

		for (int camImageX = startImage.x; camImageX < endImage.x; camImageX++)
		{
			for (int camImageY = startImage.y; camImageY < endImage.y; camImageY++)
			{
				mergedImage.at<uchar>(camImageY, camImageX) = resizedImage.at<uchar>(camImageY - startImage.y, camImageX - startImage.x);
			}
		}
	}

	imshow("Pose3D Cameras", mergedImage);
	waitKey(1);
}