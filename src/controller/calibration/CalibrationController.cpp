#include "CalibrationController.h"

CalibrationController::CalibrationController(int maxCheckboards)
{
	this->maxCheckboards = maxCheckboards;
}

int CalibrationController::getMaxCheckboards()
{
	return maxCheckboards;
}