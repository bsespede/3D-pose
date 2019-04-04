#include "CalibrationController.h"

CalibrationController::CalibrationController(int maxCheckboards) : maxCheckboards(maxCheckboards)
{
	
}

int CalibrationController::getMaxCheckboards()
{
	return maxCheckboards;
}