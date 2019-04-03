#include "Console.h"

Console::Console(AppController& AppController) : AppController(AppController)
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

	if (AppController.createScene(name))
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

	if (AppController.sceneExists(name))
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
			showCalibrateIntrinsics(name);
		}
		else if (input == '2')
		{
			showCalibrateExtrinsics(name);
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
	if (AppController.hasPreviousCapture(name))
	{
		while (true)
		{
			printf("\nThis scene has already been captured, would you like to replace it?:\n");
			printf("(1) Yes\n");
			printf("(2) No\n");

			char input = getch();

			if (input == '1')
			{
				AppController.deleteCapture(name);
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
	if (!AppController.initializeCapture(name, CaptureMode::GRAYSCALE))
	{
		showStatusMessage("Camera initialization failed\n", RED);
		return;
	}

	printf("Cameras are now recording (press any key to stop)...\n");
	AppController.startCapture(name);

	getch();

	printf("Stopped capturing frames...\n");
	AppController.stopCapture();

	printf("Processing scene...\n");
	while (!AppController.hasFinishedProcessing())
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	showStatusMessage("Scene recorded succesfully\n", GREEN);
}

void Console::showCalibrateExtrinsics(std::string name)
{
	if (AppController.hasPreviousCapture(name, CalibrationMode::EXTRINSICS))
	{
		while (true)
		{
			printf("\nThis scene already has extrinsics, would you like to replace it?:\n");
			printf("(1) Yes\n");
			printf("(2) No\n");

			char input = getch();

			if (input == '1')
			{
				AppController.deleteCapture(name, CalibrationMode::EXTRINSICS);
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

	if (calibrationMode == CalibrationMode::EXTRINSICS)
	{
		printf("\nInitializing cameras...\n");
		if (!AppController.initializeExtrinsics(name))
		{
			showStatusMessage("Camera initialization failed\n", RED);
			return;
		}

		printf("Capture empty scene (press any key when ready)...\n");
		getch();
		AppController.startCaptureFrame(name);
		showStatusMessage("Empty frame captured\n", GREEN);

		printf("\nCameras are ready for wanding (press any key when to start)...\n");
		getch();

		printf("Started capturing frames (press any key when to stop)...\n");
		AppController.startCapture(name, calibrationMode);
		getch();		

		printf("Stopped capturing frames...\n");
		AppController.stopCapture();

		printf("Processing frames...\n");
		while (!AppController.hasFinishedProcessing())
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		showStatusMessage("Calibration footage has been captured\n", GREEN);

		printf("Capture scene with axis (press any key when ready)...\n");
		getch();
		AppController.captureExtrinsicsFrame(name, "axis");
		showStatusMessage("Axis frame captured\n", GREEN);

		// TODO: Call extrinsics processing/**/

	showStatusMessage("Calibrated scene succesfully\n", GREEN);
}

void Console::showStatusMessage(std::string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}