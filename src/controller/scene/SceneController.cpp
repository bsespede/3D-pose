#include "SceneManager.h"

SceneManager::SceneManager(std::string path) : path(path), optitrackCamera(OptitrackCamera()), recording(false), processing(false)
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

bool SceneManager::hasPreviousCapture(std::string name)
{
	return boost::filesystem::exists(path + "/" + name + "/capture");
}

bool SceneManager::hasPreviousCapture(std::string name, CalibrationMode calibrationMode)
{
	return boost::filesystem::exists(path + "/" + name + "/" + calibrationMode.toString() + ".json");
}

void SceneManager::deleteCapture(std::string name)
{
	boost::filesystem::remove_all(path + "/" + name + "/capture");
}

void SceneManager::deleteCapture(std::string name, CalibrationMode calibrationMode)
{
	boost::filesystem::remove_all(path + "/" + name + "/" + calibrationMode.toString() + ".json");
}

bool SceneManager::initializeCapture(std::string name, CaptureMode captureMode)
{
	return optitrackCamera.initialize(captureMode);
}

void SceneManager::startCapture(std::string name)
{
	std::string sceneFolder = path + "/" + name + "/capture";
	boost::filesystem::create_directory(sceneFolder);

	recording = true;
	processing = true;

	std::thread captureThread = std::thread(&SceneManager::captureSceneFrames, this, name);
	captureThread.detach();
}

void SceneManager::startCapture(std::string name, CalibrationMode calibrationMode)
{
	recording = true;
	processing = true;

	std::thread captureThread = std::thread(&SceneManager::captureCalibrationFrames, this, name);
	captureThread.detach();
}

void SceneManager::captureSceneFrames(std::string name)
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

	if (!frames.empty())
	{
		for (std::pair<int, cv::Mat> keyValue : frames.front())
		{
			std::string cameraFolder = path + "/" + name + "/capture/cam-" + std::to_string(keyValue.first);
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
			std::string cameraFolder = path + "/" + name + "/capture/cam-" + std::to_string(keyValue.first);
			std::string frameFile = cameraFolder + "/" + std::to_string(frameNumber) + ".png";
			
			cv::imwrite(frameFile, keyValue.second);
		}
	}

	processing = false;

	optitrackCamera.stop();
}

void SceneManager::stopCapture()
{
	recording = false;
}

bool SceneManager::hasFinishedProcessing()
{
	return processing;
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