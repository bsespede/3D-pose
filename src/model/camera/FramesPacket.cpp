#include "FramesPacket.h

FramesPacket::FramesPacket()
{
	frames = map<int, Mat>();
}

void FramesPacket::addFrame(int camera, Mat frame)
{
	frames[camera] = frame;
}

map<int, Mat> FramesPacket::getFrames()
{
	return frames;
}

Mat getMergedFrame()
{
	int barHeight = 20;
	int cameraWidth = 213 + 2;
	int cameraHeight = 160 + 2;

	int rows = 4;
	int cols = 4;
	Mat mergedImage = Mat(cameraHeight * rows - rows + 1, cameraWidth * cols - cols + 1, CV_8UC1, Scalar(15));

	for (int u = 0; u < cols; u++)
	{
		for (int v = 0; v < rows; v++)
		{
			int correctionHorizontal = 1 * u;
			int correctionVertical = 1 * v;

			Point2i startLight = Point2i(1 + u * cameraWidth - correctionHorizontal, 1 + v * cameraHeight - correctionVertical);
			Point2i endLight = Point2i(cameraWidth - 2 + u * cameraWidth - correctionHorizontal, barHeight - 2 + v * cameraHeight - correctionVertical);
			rectangle(mergedImage, startLight, endLight, Scalar(100), FILLED);

			Point2i startMid = Point2i(2 + u * cameraWidth - correctionHorizontal, 2 + v * cameraHeight - correctionVertical);
			Point2i endMid = Point2i(cameraWidth - 3 + u * cameraWidth - correctionHorizontal, barHeight - 3 + v * cameraHeight - correctionVertical);
			rectangle(mergedImage, startMid, endMid, Scalar(75), FILLED);

			Point2i startDark = Point2i(1 + u * cameraWidth - correctionHorizontal, barHeight + v * cameraHeight - correctionVertical);
			Point2i endDark = Point2i(cameraWidth - 2 + u * cameraWidth - correctionHorizontal, cameraHeight - 2 + v * cameraHeight - correctionVertical);
			rectangle(mergedImage, startDark, endDark, Scalar(25), FILLED);

			Point2i startText = Point2i(4 + u * cameraWidth - correctionHorizontal, barHeight - 7 + v * cameraHeight - correctionVertical);
			Point2i startTextShadow = Point2i(5 + u * cameraWidth - correctionHorizontal, barHeight - 6 + v * cameraHeight - correctionVertical);
			putText(mergedImage, "Camera #" + to_string(u + v * cols), startTextShadow, FONT_HERSHEY_DUPLEX, 0.4, Scalar(0));
			putText(mergedImage, "Camera #" + to_string(u + v * cols), startText, FONT_HERSHEY_DUPLEX, 0.4, Scalar(255));
		}
	}

	for (pair<int, Mat> pair: frames) {
		int u = pair.first % cols;
		int v = pair.first / rows;

		int correctionHorizontal = 1 * u;
		int correctionVertical = 1 * v;

		Point2i startImage = Point2i(1 + u * cameraWidth - correctionHorizontal, barHeight + v * cameraHeight - correctionVertical);
		Point2i endImage = Point2i(cameraWidth - 1 + u * cameraWidth - correctionHorizontal, cameraHeight - 1 + v * cameraHeight - correctionVertical);

		Mat resizedImage;
		resize(pair.second, resizedImage, Size(cameraWidth - 1, cameraHeight - 1));

		for (int camImageX = startImage.x; camImageX < endImage.x; camImageX++)
		{
			for (int camImageY = startImage.y; camImageY < endImage.y; camImageY++)
			{
				mergedImage.at<uchar>(camImageY, camImageX) = resizedImage.at<uchar>(camImageY - startImage.y, camImageX - startImage.x);
			}
		}

		imshow("mergedImage", mergedImage);
	}
}