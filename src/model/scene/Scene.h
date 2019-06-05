#pragma once

#include <string>

class Scene
{
public:
	Scene(std::string name, std::string date);
	std::string getDate();
	std::string getName();
private:
	std::string name;
	std::string date;
};
