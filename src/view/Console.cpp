#include "Console.h"

Console::Console(ConfigController* configController)
{
	this->showPreviewOnCapture = configController->getShowPreviewOnCapture();
	this->showPreviewUI = false;
	this->appController = new AppController(configController);
	this->renderer2D = new Renderer2D(configController);
}

void Console::start()
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
		printf("(1) Scene creation\n");
		printf("(2) Scene loading\n");
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
			showCameraPreview();
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
			showSceneOptions(scene);
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
			showSceneOptions(scene);
		}
		else
		{
			showStatusMessage("Scene already exists\n", RED);
		}		
	}
}

void Console::showSceneOptions(Scene scene)
{
	while (true)
	{
		printf("\nChoose an action from the following:\n");
		printf("(1) Camera calibration\n");
		printf("(2) Motion capture\n");
		printf("(3) Back\n");

		int input = _getch();

		if (input == '1')
		{
			showCalibrationOptions(scene);
		}
		else if (input == '2')
		{
			showMocapOptions(scene);
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

void Console::showCalibrationOptions(Scene scene)
{
	while (true)
	{
		printf("\nChoose an action from the following:\n");
		printf("(1) Capture calibration\n");
		printf("(2) Process intrinsics\n");
		printf("(3) Process extrinsics\n");
		printf("(4) Preview result\n");
		printf("(5) Back\n");

		int input = _getch();

		if (input == '1')
		{
			return showCapture(scene, CaptureType::CALIBRATION);
		}
		else if (input == '2')
		{
			return showProcessCalibration(scene, CalibrationType::INTRINSICS);
		}
		else if (input == '3')
		{
			return showProcessCalibration(scene, CalibrationType::EXTRINSICS);
		}
		else if (input == '4')
		{
			return showResultPreview(scene, CaptureType::CALIBRATION);
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

void Console::showMocapOptions(Scene scene)
{
	while (true)
	{
		printf("\nChoose an action from the following:\n");
		printf("(1) Capture scene\n");
		printf("(2) Process scene\n");
		printf("(3) Preview result\n");
		printf("(4) Back\n");

		int input = _getch();

		if (input == '1')
		{
			return showCapture(scene, CaptureType::MOCAP);
		}
		else if (input == '2')
		{
			return showProcessMocap(scene);
		}
		else if (input == '3')
		{
			return showResultPreview(scene, CaptureType::MOCAP);
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


void Console::showCapture(Scene scene, CaptureType captureType)
{
	if (appController->hasCapture(scene, captureType))
	{
		showStatusMessage("There is already a capture\n", RED);
		return;
	}

	printf("\nInitializing cameras...\n");
	if (!appController->startCameras(captureType))
	{
		showStatusMessage("Camera initialization failed\n", RED);
		return;
	}

	if (showPreviewOnCapture)
	{
		showPreviewUI = true;
		thread camerasThread = thread(&Console::showCameraPreviewLoop, this);
		camerasThread.detach();
	}	

	printf("Press any key to start capturing...\n");
	_getch();
	appController->startCapturingVideo();

	printf("Press any key to stop capturing...\n");
	_getch();
	appController->stopCapturingVideo();

	if (showPreviewOnCapture)
	{
		showPreviewUI = false;
	}
	
	printf("Dumping capture to disk, please wait...\n");
	appController->stopCameras();	
	appController->saveCapture(scene, captureType);

	showStatusMessage("Scene captured succesfully\n", GREEN);
}

void Console::showProcessCalibration(Scene scene, CalibrationType calibrationType)
{
	if (!appController->hasCapture(scene, CaptureType::CALIBRATION))
	{
		showStatusMessage("There is no calibration capture to process\n", RED);
		return;
	}

	printf("\nCalculating camera %s, please wait...\n", calibrationType.toString().c_str());

	if (!appController->calibrate(scene, calibrationType))
	{
		showStatusMessage("Calibration processing failed\n", RED);
	}
	else
	{
		showStatusMessage("Calibration processing was succesfull\n", GREEN);
	}
}

void Console::showProcessMocap(Scene scene)
{
	if (!appController->hasCapture(scene, CaptureType::MOCAP))
	{
		showStatusMessage("There is no mocap capture to process\n", RED);
		return;
	}

	showStatusMessage("Mocap processing not implemented yet\n", RED);
}

void Console::showResultPreview(Scene scene, CaptureType)
{
	/*Results* results = appController->getResults(scene, operation);

	if (results == nullptr)
	{
		showStatusMessage("No results found\n", RED);
	}

	for (int cameraNumber : results->capturedCameras())
	{
		Extrinsics* extrinsics = results->getExtrinsics(cameraNumber);
		Extrinsics* intrinsics = results->getExtrinsics(cameraNumber);
	}*/
}

void Console::showCameraPreview()
{
	printf("\nInitializing cameras...\n");
	if (!appController->startCameras(CaptureType::MOCAP))
	{
		showStatusMessage("Camera initialization failed\n", RED);
		showMenu();
	}

	showPreviewUI = true;
	thread camerasThread = thread(&Console::showCameraPreviewLoop, this);
	camerasThread.detach();

	printf("Previewing cameras (press any key to stop)...\n");
	_getch();

	showPreviewUI = false;
	appController->stopCameras();

	showStatusMessage("Camera testing finished\n", GREEN);
}

void Console::showCameraPreviewLoop()
{
	while (showPreviewUI)
	{
		Packet* safeImage = appController->getSafeImage();

		if (safeImage != nullptr)
		{
			renderer2D->render(safeImage);
		}	

		appController->updateSafeImage();	

		int milisecondsToSleep = (int)(1.0 / renderer2D->getGuiFps() * 1000);
		this_thread::sleep_for(chrono::milliseconds(milisecondsToSleep));
	}
}

void Console::showStatusMessage(string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}