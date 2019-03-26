#include "Console.h"

Console::Console(SceneManager sceneManager)
{
	this->sceneManager = sceneManager;
}

void Console::main()
{
	showStatusMessage("   _____ ____  ____                  \n", BLUE);
	showStatusMessage("  |__  // __ \\/ __ \\____  ________ \n", BLUE);
	showStatusMessage("   /_ </ / / / /_/ / __ \\/ ___/ _ \\\n", BLUE);
	showStatusMessage(" ___/ / /_/ / ____/ /_/ /__  /  __/  \n", BLUE);
	showStatusMessage("/____/_____/_/    \\____/____/\\___/ \n", BLUE);
	showStatusMessage("                                     \n", BLUE);

	showMenuOptions();
}

void Console::showMenuOptions()
{
	while (true)
	{
		printf("Choose an action from the following:\n");
		printf("(1) Create scene\n");
		printf("(2) Load scene\n");
		printf("(3) Exit\n");
		printf(">");

		int option = getchar();

		if (option == '1')
		{
			showSceneCreation();
		}
		else if (option == '2')
		{
			showSceneLoad();
		}
		else if (option == '3')
		{
			return;
		}
		else
		{
			showStatusMessage("Choose a valid option\n", RED);
		}
	}
}

void Console::showSceneCreation()
{
	printf("Choose a short name for the scene:\n");
	printf(">");

	std::string name;
	std::getline(std::cin, name);

	if (sceneManager.createScene(name))
	{
		Scene scene = sceneManager.getScene(name);
		showStatusMessage("Scene created succesfully\n", GREEN);
		showSceneOperations(scene);
	}
	else
	{
		showStatusMessage("Scene creation failed\n", RED);
	}
}

void Console::showSceneLoad()
{
	printf("Type the name of the scene to load:\n");
	printf(">");

	std::string name;
	std::getline(std::cin, name);

	if (!sceneManager.sceneExists(name))
	{
		Scene scene = sceneManager.getScene(name);
		showStatusMessage("Scene loaded succesfully\n", GREEN);
		showSceneOperations(scene);
	}
	else
	{
		showStatusMessage("Scene doesn't exist\n", RED);
	}
}

void Console::showSceneOperations(Scene scene)
{
	while (true)
	{
		printf("Choose an action from the following:\n");
		printf("(1) Capture calibration scene\n");
		printf("(2) Calibrate scene\n");
		printf("(3) Capture scene\n");
		printf("(4) Generate skeleton file\n");
		printf("(5) Back\n");
		printf(">");

		int operation = getchar();

		if (operation == '1')
		{
			showRecordScene(scene, CaptureMode::UNCALIBRATED_SCENE);
		}
		else if (operation == '2')
		{
			showCalibrateScene(scene);
		}
		else if (operation == '3')
		{
			showRecordScene(scene, CaptureMode::CALIBRATED_SCENE);
		}
		else if (operation == '4')
		{
			showProcessScene(scene);
		}
		else if (operation == '5')
		{
			return;
		}
		else
		{
			showStatusMessage("Choose a valid option\n", RED);
		}
	}
}

void Console::showHasPreviousRecording(Scene scene, CaptureMode captureMode)
{
	if (sceneManager.hasPreviousRecording(scene, captureMode))
	{
		while (true)
		{
			printf("This scene already has a recording, would you like to replace it?:\n");
			printf("(1) Yes\n");
			printf("(2) No\n");
			printf(">");

			int operation = getchar();

			if (operation == '1')
			{
				showRecordScene(scene, captureMode);
			}
			else if (operation == '2')
			{
				return;
			}
			else
			{
				showStatusMessage("Choose a valid option\n", RED);
			}
		}
	}
}

void Console::showRecordScene(Scene scene, CaptureMode captureMode)
{
	printf("Initializing cameras\n");

	if (!sceneManager.initializeCameras())
	{
		showStatusMessage("Camera initialization failed\n", RED);
		return;
	}

	printf("Synchronizing cameras\n");
	if (!sceneManager.synchronizeCameras())
	{
		showStatusMessage("Camera synchronization failed\n", RED);
		return;
	};

	printf("Cameras are now recording (press any key to stop)\n");

	sceneManager.startRecording(scene, captureMode); // TODO: call en backend
	
	getchar();

	sceneManager.stopRecording();// TODO: emit signal to stop recording thread
}

void Console::showCalibrateScene(Scene scene)
{
	printf("Calibrating cameras\n");

	if (sceneManager.calibrateScene(scene))
	{
		showStatusMessage("Scene calibrated succesfully\n", GREEN);
		showSceneOperations(scene);
	}
	else
	{
		showStatusMessage("Scene calibration failed\n", RED);
	}
}

void Console::showProcessScene(Scene scene)
{
	printf("Processing scene\n");

	if (sceneManager.processScene(scene))
	{
		showStatusMessage("Scene processed succesfully\n", GREEN);
	}
	else
	{
		showStatusMessage("Scene processing failed\n", RED);
	}
}

void Console::showStatusMessage(std::string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}