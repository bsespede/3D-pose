#pragma once

#include <string>

using namespace std;

class Scene
{
public:
	Scene(string name, string date);
	string getDate();
	string getName();
private:
	string name;
	string date;
};
