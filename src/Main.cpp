#include "view/Console.h"
#include "controller/AppController.h"

int main(int argc, char **argv)
{
	AppController* appController = new AppController("../data", 20, 60);
	Console* console = new Console(appController);

	console->start();

	return 0;
}
