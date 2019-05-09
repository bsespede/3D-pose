#include "Console.h"

Console::Console(Config* config, AppController* appController)
{
	this->appController = appController;
	this->cameraRenderer = new CameraRenderer(config->getCameraWidth(), config->getCameraHeight(), config->getMaxWidth(), config->getMaxHeight(), config->getCamerasNumber(), config->getBarHeight());
	this->showPreviewOnCapture = config->getShowPreviewOnCapture();
	this->checkboardTimer = config->getCheckboardTimer();
	this->guiFps = config->getGuiFps();
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
		printf("(3) Preview cameras\n");
		printf("(4) Exit\n");

		char input = getch();

		if (input == '1')
		{
			showSceneInput(Input::CREATE);
		}
		else if (input == '2')
		{
			showSceneInput(Input::LOAD);
		}
		else if (input == '3')
		{
			showPreview();
		}
		else if (input == '4')
		{
			exit(0);
		}
		else
		{
			showStatusMessage("Choose a valid option\n", RED);
		}
	}
}

void Console::showSceneInput(Input input)
{
	printf("\nInput the name of the scene:\n");
	printf(">");

	string name;
	getline(cin, name);

	if (!appController->sceneExists(name))
	{
		if (input == Input::CREATE)
		{
			Scene scene = appController->createScene(name);
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
			Scene scene = appController->loadScene(name);
			showStatusMessage("Scene loaded succesfully\n", GREEN);
			showOperations(scene);
		}
		else
		{
			showStatusMessage("Scene already exists\n", RED);
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
			showCapture(scene, Operation::SCENE);
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
		printf("(1) Capture %s\n", operation.toString().c_str());
		printf("(2) Process %s\n", operation.toString().c_str());
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

void Console::showCapture(Scene scene, Operation operation)
{
	printf("\nInitializing cameras...\n");
	if (!appController->startCameras(operation.getCaptureMode()))
	{
		showStatusMessage("Camera initialization failed\n", RED);
		showOperationOptions(scene, operation);
	}

	if (showPreviewOnCapture) 
	{
		showCamera = true;
		thread camerasThread = thread(&Console::showPreview, this);
		camerasThread.detach();
	}	

	if (operation == Operation::EXTRINSICS)
	{
		printf("Prepare to capture empty scene (press any key to start)...\n");
		getch();
		appController->captureFrame();

		printf("Prepare for wanding (press any key to start)...\n");
		getch();
		appController->startRecordingFrames();

		printf("Recording wanding (press any key to stop)...\n");
		getch();
		appController->stopRecordingFrames();

		printf("Prepare to capture scene axis (press any key to start)...\n");
		getch();
		appController->captureFrame();
	}
	else if (operation == Operation::INTRINSICS)
	{
		printf("Prepare for checkboarding (capturing frame every 10 seconds)...\n");		
		for (int checkboardNumber = 0; checkboardNumber < appController->getMaxCheckboards(); checkboardNumber++)
		{
			this_thread::sleep_for(chrono::seconds(10));
			Beep(500, 400);
			appController->captureFrame();
			printf("Captured frame %d/%d...\n", checkboardNumber + 1, appController->getMaxCheckboards());
		}
	}
	else
	{
		printf("Prepare for capture (press any key to start)...\n");
		getch();
		appController->startRecordingFrames();

		printf("Recording scene (press any key to stop)...\n");
		getch();
		appController->stopRecordingFrames();
	}

	if (showPreviewOnCapture)
	{
		showCamera = false;
	}
	
	printf("Dumping captures to disk...\n");
	appController->stopCameras();		
	appController->dumpCapture(scene, operation);

	showStatusMessage("Scene captured succesfully\n", GREEN);
}

void Console::showProcess(Scene scene, Operation operation)
{
	if (operation == Operation::EXTRINSICS)
	{
		showStatusMessage("Extrinsic calibration not implemented yet\n", RED);
	}
	else if (operation == Operation::INTRINSICS)
	{
		printf("\nCalculating cameras intrinsics...\n");
		appController->calculateIntrinsics(scene);

		showStatusMessage("Intrinsic calibration was succesfull\n", GREEN);
	}
	else
	{
		showStatusMessage("3D pose reconstruction not implemented yet\n", RED);
	}

	showOperations(scene);
}

void Console::showPreview()
{
	printf("\nInitializing cameras...\n");
	if (!appController->startCameras(CaptureMode::GRAYSCALE))
	{
		showStatusMessage("Camera initialization failed\n", RED);
		showMenu();
	}

	showCamera = true;
	thread camerasThread = thread(&Console::showPreviewGUI, this);
	camerasThread.detach();

	printf("Previewing cameras (press any key to stop)...\n");
	getch();

	showCamera = false;
	appController->stopCameras();

	showStatusMessage("Camera testing finished\n", GREEN);
}

void Console::showPreviewGUI()
{
	int curFrame = 0;
	while (showCamera)
	{
		int milisecondsToSleep = (int)(1.0 / guiFps * 1000);
		chrono::system_clock::time_point timePoint = chrono::system_clock::now() + chrono::milliseconds(milisecondsToSleep);

		FramesPacket* safeFrame = appController->getSafeFrame();

		if (safeFrame != nullptr)
		{
			cameraRenderer->render(safeFrame);
		}	

		appController->updateSafeFrame();

		this_thread::sleep_until(timePoint);
	}
}

void Console::showStatusMessage(string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}