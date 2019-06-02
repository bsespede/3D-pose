#include "OptitrackCamera.h"

OptitrackCamera::OptitrackCamera(ConfigController* configController)
{
	this->cameraFps = configController->getCameraFps();
	this->cameraWidth = configController->getCameraWidth();
	this->cameraHeight = configController->getCameraHeight();
	this->cameraOrder = configController->getCameraOrder();
}

bool OptitrackCamera::startCameras()
{
	CameraLibrary_EnableDevelopment();
	CameraManager::X();
	CameraManager::X().WaitForInitialization();
	cameraCount = 0;

	for (int i = 0; i < list.Count(); i++)
	{
		camera[i] = CameraManager::X().GetCamera(list[i].UID());
		int cameraSerial = list[i].Serial();
		int cameraNumber = cameraOrder[cameraSerial];
		
		if (camera[i] == nullptr)
		{
			BOOST_LOG_TRIVIAL(warning) << "Couldn't connect to camera #" << cameraNumber << ": ";
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
		camera[i]->SetNumeric(true, cameraOrder[camera[i]->Serial()]);
		camera[i]->SetVideoType(Core::eVideoMode::MJPEGMode);
		camera[i]->SetMJPEGQuality(0);		
		camera[i]->SetFrameRate(cameraFps);
		camera[i]->SetLateDecompression(false);
		camera[i]->Start();	
	}

	return true;
}

Packet* OptitrackCamera::getPacket()
{
	FrameGroup* frameGroup = sync->GetFrameGroup();
	
	if (frameGroup)
	{
		if (frameGroup->Count() != cameraCount)
		{			
			frameGroup->Release();
			BOOST_LOG_TRIVIAL(warning) << "Dropped unsynced frame";
			return nullptr;
		}

		Packet* packet = new Packet();

		for (int i = 0; i < frameGroup->Count(); i++)
		{
			Frame* frame = frameGroup->GetFrame(i);
			Camera* camera = frame->GetCamera();

			int cameraId = cameraOrder[camera->Serial()];

			Mat frameMat = Mat(Size(cameraWidth, cameraHeight), CV_8UC1);
			frame->Rasterize(cameraWidth, cameraHeight, (unsigned int)frameMat.step, 8, frameMat.data);

			packet->addData(cameraId, frameMat);
			frame->Release();
		}

		frameGroup->Release();
		return packet;
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
	for (int i = 0; i < cameraCount; i++)
	{
		if (camera[i] != nullptr)
		{
			camera[i]->Release();
		}
	}

	CameraManager::X().Shutdown();
}
