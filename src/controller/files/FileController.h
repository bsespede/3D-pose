#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "model/scene/Scene.h"
#include "model/scene/enum/Operation.h"
#include "model/capture/Capture.h"
#include "model/calibration/IntrinsicCalibration.h"
#include "model/calibration/ExtrinsicCalibration.h"

using namespace std;
using namespace boost;

class FileController
{
public:
	FileController();

	// Scene
	bool sceneExists(string name);
	Scene loadScene(string name);
	Scene createScene(string name);

	// Capture
	bool hasCapture(Scene scene, Operation operation);
	void saveCapture(Scene scene, Operation operation, Capture* capture);
	vector<int> getCapturedCameras(Scene scene, Operation operation);
	bool hasCapturedFrame(Scene scene, Operation operation, int cameraNumber, int frameNumber);
	Mat getCapturedFrame(Scene scene, Operation operation, int cameraNumber, int frameNumber);

	// Calibration
	IntrinsicCalibration* getIntrinsics(int cameraNumber);
	void saveIntrinsics(map<int, IntrinsicCalibration*> calibrationResults);
	void saveExtrinsics(Scene scene, map<int, ExtrinsicCalibration*> extrinsicMatrices);
	void saveCalibrationDetections(Mat detection, Scene scene, Operation operation, int cameraNumber, int frameNumber);

	// Config file: Folder
	string getDataFolder();
	
	// Config file: GUI
	int getMaxWidth();
	int getMaxHeight();
	int getCameraHeight();
	int getCameraWidth();
	int getBarHeight();
	int getGuiFps();
	bool getShowPreviewOnCapture();

	// Config file: Cameras
	map<int, int> getCamerasOrder();
	int getCamerasNumber();
	int getCamerasFps();

	// Config file: Calibration
	int getMaxCheckboards();
	int getCheckboardTimer();
	int getCheckboardCols();
	int getCheckboardRows();
	float getCheckboardSquareLength();
	float getCheckboardMarkerLength();
	int getCheckboardWidth();
	int getCheckboardHeight();
	int getCheckboardMargin();

private:
	// Folder
	string dataFolder;

	// GUI
	int maxWidth;
	int maxHeight;
	int cameraHeight;
	int cameraWidth;
	int barHeight;
	int guiFps;
	bool showPreviewOnCapture;

	// Cameras
	map<int, int> camerasOrder;
	int camerasFps;

	// Calibration
	int maxCheckboards;
	int checkboardTimer;
	int checkboardCols;
	int checkboardRows;
	float checkboardSquareLength;
	float checkboardMarkerLength;
	int checkboardWidth;
	int checkboardHeight;
	int checkboardMargin;
};
