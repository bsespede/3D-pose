#include "backend/scene/SceneManager.h"
#include "frontend/CLI.h"

int main(int argc, char* argv[])
{
	SceneManager sceneManager("../data");
	CLI cli(sceneManager);
}
