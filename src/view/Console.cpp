#include "Console.h"
#include "model/scene/enum/CalibrationMode.h"

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
		printf("(1) Calibrate scene\n");
		printf("(2) Capture scene\n");
		printf("(3) Back\n");

		int input = getch();

		if (input == '1')
		{
			showCalibrationOptions(name); 
		}
		else if (input == '2')
		{
			showRecordScene(name);
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

void Console::showRecordScene(std::string name)
{
	if (sceneManager.hasPreviousRecording(name))
	{
		while (true)
		{
			printf("\nThis scene already has a recording, would you like to replace it?:\n");
			printf("(1) Yes\n");
			printf("(2) No\n");

			char input = getch();

			if (input == '1')
			{
				sceneManager.deleteRecording(name);
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

	printf("Cameras are now recording (press any key to stop)...\n");
	sceneManager.startRecording(name, captureMode);
	
	getch();

	printf("Stopped capturing frames...\n");
	sceneManager.stopRecording();

	printf("Processing scene...\n");
	while (!sceneManager.hasFinishedProcessing())
	{
		std::this_thread::sleep_for(1s);
	}

	showStatusMessage("Scene recorded succesfully\n", GREEN);
}

void Console::showCalibrationOptions(std::string name)
{
	while (true)
	{
		printf("\nChoose an action from the following:\n");
		printf("(1) Intrinsic calibration\n");
		printf("(2) Extrinsic calibration\n");
		printf("(5) Back\n");

		int input = getch();

		if (input == '1')
		{
			showCalibration(name, CalibrationMode::INTRINSICS);
		}
		else if (input == '2')
		{
			showCalibration(name, CalibrationMode::EXTRINSICS);
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

void Console::showCalibrateScene(std::string name, CalibrationMode calibrationMode)
{
	if (sceneManager.hasPreviousCalibration(name, calibrationMode))
	{
		while (true)
		{
			printf("\nThis scene already has an %s calibration, would you like to replace it?:\n", calibrationMode.toString());
			printf("(1) Yes\n");
			printf("(2) No\n");

			char input = getch();

			if (input == '1')
			{
				sceneManager.deleteCalibration(name, calibrationMode);
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
	sceneManager.synchronizeCameras(calibrationMode.getCaptureMode());

	printf("Cameras are now capturing frames for calibration (press any key to stop)...\n");
	sceneManager.startCalibrating(name, calibrationMode);

	getch();

	printf("Stopped capturing frames...\n");
	sceneManager.stopCalibrating();

	printf("Processing calibration...\n");
	while (!sceneManager.hasFinishedProcessing())
	{
		std::this_thread::sleep_for(1s);
	}

	if (calibrationMode == CalibrationMode::EXTRINSICS)
	{	
		printf("Put the axis in the scene (press any key when ready)...\n");
		
		getch();
		sceneManager.captureCalibrationAxis(name);
	}

	showStatusMessage("Calibrated scene succesfully\n", GREEN);
}

void Console::showStatusMessage(std::string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}