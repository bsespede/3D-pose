#pragma once

#include <string>

using namespace std;

class AppController
{
private:
	void loggerInit();
	string dataPath;
public:
	AppController(string dataPath);
	
};
