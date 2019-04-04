#pragma once

class CalibrationController
{
public:
	CalibrationController(int maxCheckboards);
	int getMaxCheckboards();
private:
	int maxCheckboards;
};