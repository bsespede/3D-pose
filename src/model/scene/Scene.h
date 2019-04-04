#pragma once

#include <string>

using namespace std;

class Scene
{
private:
	string name;
	string date;
public:
	Scene(string name, string date);
	string getDate();
	string getName();
};
