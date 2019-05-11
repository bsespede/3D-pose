#include "view/Console.h"
#include "controller/AppController.h"

int main(int argc, char **argv)
{
	FileController* fileController = new FileController();
	AppController* appController = new AppController(fileController);
	Console* console = new Console(fileController);
	console->start();

	return 0;
}