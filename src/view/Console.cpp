#include "Console.h"

Console::Console(ConfigController* configController)
{
	this->showPreviewOnCapture = configController->getShowPreviewOnCapture();
	this->showPreviewUI = false;
	this->appController = new AppController(configController);
	this->renderer2D = new Renderer2D(configController);
	this->renderer3D = new Renderer3D(configController);
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
		printf("(1) Create scene\n");
		printf("(2) Import scene\n");
		printf("(3) Test cameras\n");
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

	std::string name;
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
		printf("(4) Process board pose\n");
		printf("(5) Preview result\n");
		printf("(6) Back\n");

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
			return showProcessCalibration(scene, CalibrationType::POSES);
		}
		else if (input == '5')
		{
			return showResultPreview(scene, CaptureType::CALIBRATION);
		}
		else if (input == '6')
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
		std::thread camerasThread = std::thread(&Console::showCameraPreviewLoop, this);
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

void Console::showResultPreview(Scene scene, CaptureType captureType)
{
	if (!appController->hasCapture(scene, captureType))
	{
		showStatusMessage("There is no capture to preview\n", RED);
		return;
	}

	Video3D* result = appController->getResult(scene, captureType);

	if (result == nullptr)
	{
		showStatusMessage("Results are not available, process first\n", RED);
		return;
	}

	renderer3D->render(result);
	delete result;

	showStatusMessage("Preview finished succesfully\n", GREEN);
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
	std::thread camerasThread = std::thread(&Console::showCameraPreviewLoop, this);
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
	}
}

void Console::showStatusMessage(std::string message, int fontColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fontColor);
	printf("%s", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
}