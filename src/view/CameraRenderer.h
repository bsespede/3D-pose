#pragma once

class CameraRenderer
{
private:
	int barHeight;
	int cameraWidth;
	int cameraHeight;
	int rows;
	int cols;
public:
	CameraRenderer(int barHeight, int cameraHeight, int cameraWidth, int rows, int cols);
	void render(FramesPacket framesPacket);
};
