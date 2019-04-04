#pragma once

class CalibrationController
{
private:
	int maxCheckboards;
public:
	CalibrationController(int maxCheckboards);
	int getMaxCheckboards();
};
