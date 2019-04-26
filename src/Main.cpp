#include "view/Console.h"
#include "controller/AppController.h"
#include "model/util/Config.h"

int main(int argc, char **argv)
{
	Config* config = new Config();
	AppController* appController = new AppController(config);
	Console* console = new Console(config, appController);
	console->start();

	return 0;
}