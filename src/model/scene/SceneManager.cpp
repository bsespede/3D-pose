#include "SceneManager.h"

SceneManager::SceneManager(std::string path) : path(path), optitrackCamera(OptitrackCamera()), recording(false)
{
	loggerInit();
}

bool SceneManager::createScene(std::string name)
{
	if (!sceneExists(name))
	{
		std::string sceneFolder = path + "/" + name;
		boost::filesystem::create_directory(sceneFolder);
		return true;
	}
	else
	{
		return false;
	}
}

bool SceneManager::sceneExists(std::string name)
{
	return boost::filesystem::exists(path + "/" + name);
}

bool SceneManager::hasPreviousRecording(std::string name, CaptureMode captureMode)
{
	std::string captureModeFolder = captureMode == CaptureMode::UNCALIBRATED_CAPTURE ? "calibration" : "capture";
	return boost::filesystem::exists(path + "/" + name + "/" + captureModeFolder);
}

void SceneManager::deleteRecording(std::string name, CaptureMode captureMode)
{
	std::string captureModeFolder = captureMode == CaptureMode::UNCALIBRATED_CAPTURE ? "calibration" : "capture";
	boost::filesystem::remove_all(path + "/" + name + "/" + captureModeFolder);
}

bool SceneManager::initializeCameras()
{
	return optitrackCamera.initialize();
}

void SceneManager::synchronizeCameras(CaptureMode captureMode)
{
	optitrackCamera.start(captureMode);
}

void SceneManager::startRecording(std::string name, CaptureMode captureMode)
{
	recording = true;
	std::thread captureThread = std::thread(&SceneManager::captureFrames, this, name, captureMode);
	captureThread.detach();
}

void SceneManager::captureFrames(std::string name, CaptureMode captureMode)
{
	std::vector<std::map<int, cv::Mat>> frames = std::vector<std::map<int, cv::Mat>>();

	while (recording)
	{
		int milisecondsToSleep = (int)(1.0 / CAPTURE_FPS);
		std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now() + std::chrono::milliseconds(milisecondsToSleep);

		std::map<int, cv::Mat> frame = optitrackCamera.capture();
		frames.push_back(frame);

		std::this_thread::sleep_until(timePoint);
	}

	std::string captureModeFolder = captureMode == CaptureMode::UNCALIBRATED_CAPTURE ? "calibration" : "capture";

	if (!frames.empty())
	{
		for (std::pair<int, cv::Mat> keyValue : frames.front())
		{
			std::string cameraFolder = path + "/" + name + "/" + captureModeFolder + "/cam-" + std::to_string(keyValue.first);
			boost::filesystem::create_directory(cameraFolder);
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Didnt capture any frame";
	}

	for (int frameNumber = 0; frameNumber < frames.size(); frameNumber++)
	{
		std::map<int, cv::Mat> frame = frames[frameNumber];

		for (std::pair<int, cv::Mat> keyValue : frame)
		{
			std::string cameraFolder = path + "/" + name + "/" + captureModeFolder + "/cam-" + std::to_string(keyValue.first);
			std::string frameFile = cameraFolder + "/" + std::to_string(frameNumber) + ".png";
			
			cv::imwrite(frameFile, keyValue.second);
		}
	}
}

void SceneManager::stopRecording()
{
	recording = false;
}

bool SceneManager::calibrateScene(std::string name)
{
	return false;
}

bool SceneManager::processScene(std::string name)
{
	return false;
}

void SceneManager::loggerInit()
{
	boost::log::add_file_log
	(
		boost::log::keywords::file_name = "errors.log",
		boost::log::keywords::format =
		(
			boost::log::expressions::stream
			<< "[" << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
			<< "] [" << boost::log::trivial::severity
			<< "] " << boost::log::expressions::smessage
			)
	);

	boost::log::add_common_attributes();
}