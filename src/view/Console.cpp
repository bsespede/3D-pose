#include "Console.h"

Console::Console(SceneManager& sceneManager) : sceneManager(sceneManager)
{
}

void Console::start()
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
		printf("\nChoose an action from the following:\n");
		printf("(1) Create scene\n");
		printf("(2) Load scene\n");
		printf("(3) Exit\n");

		char input = getch();

		if (input == '1')
		{
			showSceneCreation();
		}
		else if (input == '2')
		{
			showSceneLoad();
		}
		else if (input == '3')
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
	printf("\nChoose a short name for the scene:\n");
	printf(">");

	std::string name;
	std::getline(std::cin, name);

	if (sceneManager.createScene(name))
	{
		showStatusMessage("Scene created succesfully\n", GREEN);
		showSceneOperations(name);
	}
	else
	{
		showStatusMessage("Scene creation failed\n", RED);
	}
}

void Console::showSceneLoad()
{
	printf("\nType the name of the scene to load:\n");
	printf(">");

	std::string name;
	std::getline(std::cin, name);

	if (sceneManager.sceneExists(name))
	{
		
		showStatusMessage("Scene loaded succesfully\n", GREEN);
		showSceneOperations(name);
	}
	else
	{
		showStatusMessage("Scene doesn't exist\n", RED);
	}
}

void Console::showSceneOperations(std::string name)
{
	while (true)
	{
		printf("\nChoose an action from the following:\n");
		printf("(1) Capture calibration scene\n");
		printf("(2) Calibrate scene\n");
		printf("(3) Capture mocap scene\n");
		printf("(4) Generate skeleton file\n");
		printf("(5) Back\n");

		int input = getch();

		if (input == '1')
		{
			showRecordScene(name, CaptureMode::UNCALIBRATED_CAPTURE);
		}
		else if (input == '2')
		{
			showCalibrateScene(name);
		}
		else if (input == '3')
		{
			showRecordScene(name, CaptureMode::CALIBRATED_CAPTURE);
		}
		else if (input == '4')
		{
			showProcessScene(name);
		}
		else if (input == '5')
		{
			return;
		}
		else
		{
			showStatusMessage("Choose a valid option\n", RED);
		}
	}
}

void Console::showRecordScene(std::string name, CaptureMode captureMode)
{
	if (sceneManager.hasPreviousRecording(name, captureMode))
	{
		while (true)
		{
			printf("\nThis scene already has a recording, would you like to replace it?:\n");
			printf("(1) Yes\n");
			printf("(2) No\n");

			char input = getch();

			if (input == '1')
			{
				sceneManager.deleteRecording(name, captureMode);
			}
			else if (input == '2')
			{
				return;
			}
			else
			{
				showStatusMessage("Choose a valid option\n", RED);
			}
		}
	}

	printf("\nInitializing cameras...\n");

	if (!sceneManager.initializeCameras())
	{
		showStatusMessage("Camera initialization failed\n", RED);
		return;
	}

	printf("Synchronizing cameras...\n");
	sceneManager.synchronizeCameras(captureMode);

	printf("Cameras are now recording (press any key to stop)\n");

	sceneManager.startRecording(name, captureMode);
	
	getch();

	sceneManager.stopRecording();
}

void Console::showCalibrateScene(std::string name)
{
	printf("\nCalibrating cameras...\n");

	if (sceneManager.calibrateScene(name))
	{
		showStatusMessage("Scene calibrated succesfully\n", GREEN);
		showSceneOperations(name);
	}
	else
	{
		showStatusMessage("Scene calibration failed\n", RED);
	}
}

void Console::showProcessScene(std::string name)
{
	printf("Processing scene...\n");

	if (sceneManager.processScene(name))
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
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}