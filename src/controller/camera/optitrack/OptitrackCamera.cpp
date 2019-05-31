#include "OptitrackCamera.h"

OptitrackCamera::OptitrackCamera(FileController* fileController)
{
	this->camerasFps = fileController->getCamerasFps();
	this->camerasOrder = fileController->getCamerasOrder();
}

bool OptitrackCamera::startCameras(Core::eVideoMode mode)
{
	CameraLibrary_EnableDevelopment();
	CameraManager::X();
	CameraManager::X().WaitForInitialization();
	cameraCount = 0;

	for (int i = 0; i < list.Count(); i++)
	{
		camera[i] = CameraManager::X().GetCamera(list[i].UID());
		int cameraSerial = list[i].Serial();
		int cameraNumber = camerasOrder[cameraSerial];
		

		if (camera[i] == nullptr)
		{
			BOOST_LOG_TRIVIAL(warning) << "Couldn't connect to camera #" << cameraNumber << " (" << cameraSerial << ")";
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "Connected to camera #" << cameraNumber << " (" << cameraSerial << ")";
			cameraCount++;
		}
	}

	if (cameraCount == 0)
	{
		BOOST_LOG_TRIVIAL(warning) << "Couldn't connect to any camera";
		return false;
	}

	if (cameraCount != list.Count())
	{
		shutdownCameras();
		return false;
	}

	sync = cModuleSync::Create();

	if (sync == nullptr)
	{
		BOOST_LOG_TRIVIAL(error) << "Couldn't create sync group";
		return false;
	}

	for (int i = 0; i < cameraCount; i++)
	{
		sync->AddCamera(camera[i]);
	}

	for (int i = 0; i < cameraCount; i++)
	{
		camera[i]->SetMJPEGQuality(1);
		camera[i]->SetVideoType(mode);		
		camera[i]->SetExposure(camera[i]->MaximumExposureValue() * 0.25);
		camera[i]->SetFrameRate(camerasFps);
		camera[i]->SetFrameDecimation(camerasFps);
		camera[i]->SetNumeric(true, camerasOrder[camera[i]->Serial()]);
		camera[i]->Start();	
	}

	return true;
}

FramesPacket* OptitrackCamera::captureFramesPacket()
{
	FrameGroup* frameGroup = sync->GetFrameGroup();
	
	if (frameGroup)
	{
		FramesPacket* framesPacket = new FramesPacket();

		if (frameGroup->Count() != cameraCount)
		{			
			frameGroup->Release();
			BOOST_LOG_TRIVIAL(warning) << "Dropped unsynced frame";
			return nullptr;
		}

		for (int i = 0; i < frameGroup->Count(); i++)
		{
			Frame* frame = frameGroup->GetFrame(i);
			Camera* camera = frame->GetCamera();

			int cameraId = camerasOrder[camera->Serial()];
			int cameraWidth = camera->Width();
			int cameraHeight = camera->Height();

			Mat frameMat = Mat(Size(cameraWidth, cameraHeight), CV_8UC1);
			frame->Rasterize(cameraWidth, cameraHeight, frameMat.step, 8, frameMat.data);

			framesPacket->addFrame(cameraId, frameMat);
			frame->Release();
		}

		frameGroup->Release();
		return framesPacket;
	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "Empty packet";
	}

	return nullptr;
}

void OptitrackCamera::stopCameras()
{
	for (int i = 0; i < cameraCount; i++)
	{
		camera[i]->Stop();
	}

	sync->RemoveAllCameras();
	cModuleSync::Destroy(sync);

	shutdownCameras();
}

void OptitrackCamera::shutdownCameras()
{
	for (int i = 0; i < list.Count(); i++)
	{
		if (camera[i] != nullptr)
		{
			camera[i]->Release();
		}
	}

	CameraManager::X().Shutdown();

	while (!CameraManager::X().AreCamerasShutdown())
	{
		this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	CameraManager::X().DestroyInstance();
}
