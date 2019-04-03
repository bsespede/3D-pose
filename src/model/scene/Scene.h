#pragma once

#include <string>

using namespace std;

class Scene
{
private:
	const string name;
	const string date;
public:
	Scene(string name, string date);
	string getDate() const;
	string getName() const;
};
