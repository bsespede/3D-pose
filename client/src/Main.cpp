#include "view/Console.h"
#include "model/AppController.h"
#include "model/config/ConfigController.h"

int main(int argc, char **argv)
{
	ConfigController* configController = new ConfigController();
	AppController* appController = new AppController(configController);
	Console* console = new Console(configController);
	console->start();

	return 0;
}