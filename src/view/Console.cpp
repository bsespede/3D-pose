#include "Console.h"

Console::Console(AppController& appController)
{
	this->appController = appController;
	this->showCamera = false;
}

void Console::start()
{
	showStatusMessage("   _____ ____  ____                  \n", BLUE);
	showStatusMessage("  |__  // __ \\/ __ \\____  ________ \n", BLUE);
	showStatusMessage("   /_ </ / / / /_/ / __ \\/ ___/ _ \\\n", BLUE);
	showStatusMessage(" ___/ / /_/ / ____/ /_/ /__  /  __/  \n", BLUE);
	showStatusMessage("/____/_____/_/    \\____/____/\\___/ \n", BLUE);
	showStatusMessage("                                     \n", BLUE);

	showMenu();
}

void Console::showMenu()
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
			showInputName(Input::CREATE);
		}
		else if (input == '2')
		{
			showInputName(Input::LOAD);
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

void Console::showInputName(Input input)
{
	printf("\nInput the name of the scene:\n");
	printf(">");

	string name;
	getline(cin, name);

	if (!appController.sceneExists(name))
	{
		if (input == Input::CREATE)
		{
			Scene scene = appController.createScene(name);
			showStatusMessage("Scene created succesfully\n", GREEN);
			showOperations(scene);
		}
		else
		{
			showStatusMessage("Scene doesn't exist\n", RED);
		}
		
	}
	else
	{
		if (input == Input::LOAD)
		{
			Scene scene = appController.loadScene(name);
			showStatusMessage("Scene loaded succesfully\n", GREEN);
			showOperations(scene);
		}
		else
		{
			showStatusMessage("Scene creation failed\n", RED);
		}		
	}
}

void Console::showOperations(Scene scene)
{
	while (true)
	{
		printf("\nChoose an action from the following:\n");
		printf("(1) Intrinsics\n");
		printf("(2) Extrinsics\n");
		printf("(3) Scene\n");
		printf("(4) Back\n");

		int input = getch();

		if (input == '1')
		{
			showOperationOptions(scene, Operation::INTRINSICS);
		}
		else if (input == '2')
		{
			showOperationOptions(scene, Operation::EXTRINSICS);
		}
		else if (input == '3')
		{
			showOperationOptions(scene, Operation::SCENE);
		}
		else if (input == '4')
		{
			showMenu();
		}
		else
		{
			showStatusMessage("Choose a valid option\n", RED);
		}
	}
}

void Console::showOperationOptions(Scene scene, Operation operation)
{
	while (true)
	{
		printf("\nChoose an action from the following:\n");
		printf("(1) Capture %s\n", operation.toString());
		printf("(2) Process %s\n", operation.toString());
		printf("(3) Back\n");

		int input = getch();

		if (input == '1')
		{
			showCapture(scene, operation);
		}
		else if (input == '2')
		{
			showProcess(scene, operation);
		}
		else if (input == '3')
		{
			showOperations(scene);
		}
		else
		{
			showStatusMessage("Choose a valid option\n", RED);
		}
	}
}

void Console::showOverwrite(Scene scene, Operation operation)
{
	if (appController.hasCapture(scene, operation))
	{
		while (true)
		{
			printf("\nThe %s has already been captured, would you like to overwrite the capture?:\n", operation.toString());
			printf("(1) Yes\n");
			printf("(2) No\n");

			char input = getch();

			if (input == '1')
			{
				appController.deleteCapture(scene, operation);
			}
			else if (input == '2')
			{
				showOperationOptions(scene, operation);
			}
			else
			{
				showStatusMessage("Choose a valid option\n", RED);
			}
		}
	}
}

void Console::showCapture(Scene scene, Operation operation)
{
	showOverwrite(scene, operation);

	printf("\nInitializing cameras...\n");
	if (!appController.startCameras(scene, operation))
	{
		showStatusMessage("Camera initialization failed\n", RED);
		showOperationOptions(scene, operation);
	}

	showCamera = true;
	thread captureThread = thread(&Console::showCamera, this);
	captureThread.detach();

	if (operation == Operation::EXTRINSICS)
	{
		printf("Prepare to capture empty scene (press any key to start)...\n");
		getch();
		appController.captureFrame(scene, operation);

		printf("Prepare for wanding (press any key to start)...\n");
		getch();
		appController.startRecordingFrames(scene, operation);

		printf("Recording wanding (press any key to stop)...\n");
		getch();
		appController.stopRecordingFrames(scene, operation);

		printf("Prepare to capture scene axis (press any key to start)...\n");
		getch();
		appController.captureFrame(scene, operation);
	}
	else if (operation == Operation::INTRINSICS)
	{
		printf("Prepare for checkboarding (say 'okay' to capture frame)...\n");		
		for (int checkboardNumber = 0; checkboardNumber < appController.getMaxCheckboards(); checkboardNumber++)
		{
			if (listener.say("okay")) // should be blocking call
			{
				appController.captureFrame(scene, operation);
				printf("Captured checkboard %d/%d...", checkboardNumber, appController.getMaxCheckboards());
				checkboardNumber++;
			}			
		}
	}
	else
	{
		printf("Prepare for capture (press any key to start)...\n");
		getch();
		appController.startRecordingFrames(scene, operation);

		printf("Recording scene (press any key to stop)...\n");
		getch();
		appController.stopRecordingFrames(scene, operation);
	}

	showCamera = false;

	printf("Dumping captures to disk...\n");
	appController.dumpCapture(scene, operation);

	showStatusMessage("Scene recorded succesfully\n", GREEN);
}

void Console::showCameras()
{
	int cameraFps = appController.getCameraFps();

	while (showCamera)
	{
		int milisecondsToSleep = (int)(1.0 / cameraFps) * 1000;
		chrono::system_clock::time_point timePoint = chrono::system_clock::now() + chrono::milliseconds(milisecondsToSleep);

		Mat currentFrame = appController.getCurrentFrame();
		imshow("Current frame", currentFrame);

		this_thread::sleep_until(timePoint);
	}
}

void Console::showProcess(Scene scene, Operation operation)
{
	// TODO
	showStatusMessage("Processing not implemented yet\n", RED);
}

void Console::showStatusMessage(string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}