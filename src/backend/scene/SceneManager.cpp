#include "SceneManager.h"

SceneManager::SceneManager(std::string path)
{
	this->path = path;
}

bool SceneManager::createScene(std::string name)
{
	// create folder
	// create json
}

bool SceneManager::sceneExists(std::string name)
{
	// return scene exists
}

Scene SceneManager::getScene(std::string name)
{
	// load config file
	// 
}

bool SceneManager::initializeCameras();
bool SceneManager::synchronizeCameras();
void SceneManager::startRecording(std::string name, CaptureMode captureMode);
void SceneManager::stopRecording();
bool SceneManager::calibrateScene(std::string name);
bool SceneManager::processScene(std::string name);

void SceneManager::readConfigFile(std::string configFile)
{
	std::filesystem::path configFilePath = configFile;

	if (std::filesystem::exists(configFilePath))
	{
		boost::property_tree::ptree root;
		boost::property_tree::read_json(configFilePath, root);

		this->name = root.get<std::string>("name");
		this->path = configFilePath.parent_path().parent_path().u8string();
		this->status = static_cast<Status>(root.get<int>("status"));
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Config file doesnt exist";
	}
}

void SceneManager::writeConfigFile()
{
	boost::property_tree::ptree root;

	root.put("name", name);
	root.put("status", status);

	std::string configFilePath = path + "/" + name + "/config.json";
	boost::property_tree::write_json(configFilePath, root);
}

SceneManager::SceneManager(std::string path)
{
	this->path = path;
}
