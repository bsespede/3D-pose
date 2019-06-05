#include "Renderer2D.h"

Renderer2D::Renderer2D(ConfigController* configController)
{
	this->guiFps = configController->getGuiFps();
	this->cameraNumber = configController->getCameraNumber();
	this->barHeight = 20;	
	this->prop = 1.0f;

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	this->maxWidth = desktop.right;
	this->maxHeight = desktop.bottom;

	calculateProportions(prop);
}

void Renderer2D::calculateProportions(float prop)
{
	int curWidth = (int)(maxWidth * prop) + 2;
	int curHeight = (int)(maxHeight * prop) + barHeight + 4;
	int numberWidth = (int)((float)maxWidth / (curWidth + 1));
	int numberHeight = (int)((float)maxHeight / (curHeight + 1));
	int maxCameras = numberWidth * numberHeight;

	if (maxCameras < cameraNumber)
	{
		calculateProportions(prop - 0.1f);
	}
	else
	{
		this->cameraWidth = (int)(maxWidth * prop);
		this->cameraHeight = (int)(maxHeight * prop);
		this->rows = numberHeight;
		this->cols = numberWidth;
	}
}

void Renderer2D::render(Packet* packet)
{
	int resizedWidth = (int)(cameraWidth * prop);
	int resizedHeight = (int)(cameraHeight * prop);
	int totalWidth = cols * (resizedWidth + 2) + 1;
	int totalHeight = rows * (resizedHeight + barHeight + 4) + 1;

	cv::Mat mergedImage = cv::Mat(totalHeight, totalWidth, CV_8UC1, cv::Scalar(0));

	for (int u = 0; u < cols; u++)
	{
		for (int v = 0; v < rows; v++)
		{
			int startHorizontal = u * (resizedWidth + 2);
			int startVertical = v * (resizedHeight + barHeight + 4);

			cv::Point2i startLight = cv::Point2i(startHorizontal + 1, startVertical + 1);
			cv::Point2i endLight = cv::Point2i(startLight.x + resizedWidth, startLight.y + barHeight);
			rectangle(mergedImage, startLight, endLight, cv::Scalar(115), cv::FILLED);

			cv::Point2i startMid = cv::Point2i(startLight.x + 1, startLight.y + 1);
			cv::Point2i endMid = cv::Point2i(endLight.x - 1, endLight.y - 1);
			rectangle(mergedImage, startMid, endMid, cv::Scalar(75), cv::FILLED);

			cv::Point2i startDark = cv::Point2i(startLight.x, endLight.y + 2);
			cv::Point2i endDark = cv::Point2i(startDark.x + resizedWidth, startDark.y + resizedHeight);
			rectangle(mergedImage, startDark, endDark, cv::Scalar(25), cv::FILLED);
		}
	}

	for (std::pair<int, cv::Mat> pair : packet->getData())
	{
		int u = pair.first % cols;
		int v = pair.first / cols;

		int startHorizontal = u * (resizedWidth + 2);
		int startVertical = v * (resizedHeight + barHeight + 4);
		cv::Point2i startImage = cv::Point2i(startHorizontal + 1, startVertical + 1 + barHeight + 2);

		cv::Mat resizedImage;
		resize(pair.second, resizedImage, cv::Size(resizedWidth, resizedHeight));
		resizedImage.copyTo(mergedImage(cv::Rect(startImage.x, startImage.y, resizedWidth, resizedHeight)));

		cv::Point2i startText = cv::Point2i(startHorizontal + 5, startVertical + 15);
		cv::Point2i startTextShadow = cv::Point2i(startHorizontal + 6, startVertical + 16);
		putText(mergedImage, "Camera #" + std::to_string(pair.first), startTextShadow, cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(0));
		putText(mergedImage, "Camera #" + std::to_string(pair.first), startText, cv::FONT_HERSHEY_DUPLEX, 0.4, cv::Scalar(255));
	}

	imshow("Pose3D Cameras", mergedImage);
	cv::waitKey(1);
}

int Renderer2D::getGuiFps()
{
	return guiFps;
}