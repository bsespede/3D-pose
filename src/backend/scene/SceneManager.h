#pragma once

#include <string>
#include <filesystem>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "enum/CaptureMode.h"
#include "Scene.h"

class SceneManager
{
private:
	std::string path;
	void readConfigFile(std::string configFile);
	void writeConfigFile();
public:
	SceneManager(std::string path);
	bool sceneExists(std::string name);
	bool createScene(std::string name);
	Scene getScene(std::string name);
	bool initializeCameras();
	bool synchronizeCameras();
	void startRecording(Scene scene, CaptureMode captureMode);
	void stopRecording();
	bool calibrateScene(Scene scene);
	bool processScene(Scene scene);
};
