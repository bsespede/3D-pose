#include "model/scene/SceneManager.h"
#include "view/Console.h"

int main(int argc, char* argv[])
{
	SceneManager sceneManager("../data");
	Console console(sceneManager);	
	console.start();
}