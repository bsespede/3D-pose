#include "Console.h"

Console::Console(ConfigController* configController)
{
	this->configController = configController;
	this->appController = new AppController(configController);
	this->renderer2D = new Renderer2D(configController);
	this->showCamera = false;
}

void Console::loopUI()
{
	showLogo();
	showMenu();
}

void Console::showLogo()
{
	showStatusMessage("   _____ ____  ____                  \n", BLUE);
	showStatusMessage("  |__  // __ \\/ __ \\____  ________ \n", BLUE);
	showStatusMessage("   /_ </ / / / /_/ / __ \\/ ___/ _ \\\n", BLUE);
	showStatusMessage(" ___/ / /_/ / ____/ /_/ /__  /  __/  \n", BLUE);
	showStatusMessage("/____/_____/_/    \\____/____/\\___/ \n", BLUE);
	showStatusMessage("                                     \n", BLUE);
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

		char input = _getch();

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
			return;
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

	if (!appController->hasScene(name))
	{
		if (input == Input::CREATE)
		{
			Scene scene = appController->saveScene(name);
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
			Scene scene = appController->getScene(name);
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

		int input = _getch();

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
			return;
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

		int input = _getch();

		if (input == '1')
		{
			return showCapture(scene, operation);
		}
		else if (input == '2')
		{
			return showProcess(scene, operation);
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

void Console::showCapture(Scene scene, Operation operation)
{
	if (appController->hasCapture(scene, operation))
	{
		showStatusMessage("Scene already has a capture\n", RED);
		return;
	}

	printf("\nInitializing cameras...\n");
	if (!appController->startCameras())
	{
		showStatusMessage("Camera initialization failed\n", RED);
		return;
	}

	if (configController->getShowPreviewOnCapture())
	{
		showCamera = true;
		thread camerasThread = thread(&Console::showPreviewLoop, this);
		camerasThread.detach();
	}	

	printf("Prepare for capture (press any key to start)...\n");
	_getch();
	appController->startCapturingVideo();

	printf("Recording scene (press any key to stop)...\n");
	_getch();
	appController->stopCapturingVideo();

	if (configController->getShowPreviewOnCapture())
	{
		showCamera = false;
	}
	
	printf("Dumping captures to disk...\n");
	appController->stopCameras();	
	appController->saveCapture(scene, operation);

	showStatusMessage("Scene captured succesfully\n", GREEN);
}

void Console::showProcess(Scene scene, Operation operation)
{
	if (!appController->hasCapture(scene, operation))
	{
		showStatusMessage("There is no capture to process\n", RED);
		return;
	}

	if (operation == Operation::INTRINSICS)
	{
		printf("\nCalculating cameras intrinsics...\n");

		if (!appController->calibrate(scene, Operation::INTRINSICS))
		{
			showStatusMessage("Intrinsic calibration failed\n", RED);
		}
		else
		{
			showStatusMessage("Intrinsic calibration was succesfull\n", GREEN);
		}
	}
	else if (operation == Operation::EXTRINSICS)
	{
		printf("\nCalculating cameras extrinsics...\n");

		if (!appController->calibrate(scene, Operation::EXTRINSICS))
		{
			showStatusMessage("Extrinsic calibration failed\n", RED);
		}
		else
		{
			showStatusMessage("Intrinsic calibration was succesfull\n", GREEN);
		}
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
	if (!appController->startCameras())
	{
		showStatusMessage("Camera initialization failed\n", RED);
		showMenu();
	}

	showCamera = true;
	thread camerasThread = thread(&Console::showPreviewLoop, this);
	camerasThread.detach();

	printf("Previewing cameras (press any key to stop)...\n");
	_getch();

	showCamera = false;
	appController->stopCameras();

	showStatusMessage("Camera testing finished\n", GREEN);
}

void Console::showPreviewLoop()
{
	while (showCamera)
	{
		Packet* safeImage = appController->getSafeImage();

		if (safeImage != nullptr)
		{
			renderer2D->render(safeImage);
		}	

		appController->updateSafeImage();	

		int milisecondsToSleep = (int)(1.0 / configController->getGuiFps() * 1000);
		this_thread::sleep_for(chrono::milliseconds(milisecondsToSleep));
	}
}

void Console::showStatusMessage(string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}