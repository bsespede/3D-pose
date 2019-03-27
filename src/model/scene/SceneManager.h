#pragma once

#include <string>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include "model/camera/OptitrackCamera.h"
#include "model/scene/enum/CaptureMode.h"

#define CAPTURE_FPS 30

class SceneManager
{
private:
	std::string path;
	OptitrackCamera optitrackCamera;
	std::atomic<bool> recording;
	void loggerInit();
public:
	SceneManager(std::string path);
	bool sceneExists(std::string name);
	bool createScene(std::string name);
	bool hasPreviousRecording(std::string name, CaptureMode captureMode);
	void deleteRecording(std::string name, CaptureMode captureMode);
	bool initializeCameras();
	void synchronizeCameras(CaptureMode captureMode);	
	void startRecording(std::string name, CaptureMode captureMode);
	void captureFrames(std::string name, CaptureMode captureMode);
	void stopRecording();
	bool calibrateScene(std::string name);
	bool processScene(std::string name);
};
