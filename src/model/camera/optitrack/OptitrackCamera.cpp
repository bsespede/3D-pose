#include "OptitrackCamera.h"

OptitrackCamera::OptitrackCamera(ConfigController* configController)
{
	this->cameraFps = configController->getCameraFps();
	this->cameraData = configController->getCameraData();
}

bool OptitrackCamera::startCameras()
{
	CameraLibrary_EnableDevelopment();
	CameraLibrary::CameraManager::X();
	CameraLibrary::CameraManager::X().WaitForInitialization();
	cameraCount = 0;
	cameraCountActive = 0;

	for (int i = 0; i < list.Count(); i++)
	{
		camera[i] = CameraLibrary::CameraManager::X().GetCamera(list[i].UID());
		int cameraId = cameraData[list[i].Serial()].first;
		
		if (camera[i] == nullptr)
		{
			BOOST_LOG_TRIVIAL(warning) << "Couldn't connect to camera " << cameraId;
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "Connected to camera " << cameraId;
			cameraCount++;
		}
	}

	if (cameraCount == 0)
	{
		BOOST_LOG_TRIVIAL(warning) << "Couldn't connect to any camera";
		shutdownCameras();
		return false;
	}

	if (cameraCount != list.Count())
	{
		BOOST_LOG_TRIVIAL(warning) << "Couldnt connect to all cameras, shutting down";
		shutdownCameras();
		return false;
	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "Connected to " << cameraCount << " cameras";
	}

	sync = CameraLibrary::cModuleSync::Create();

	if (sync == nullptr)
	{
		BOOST_LOG_TRIVIAL(error) << "Couldn't create sync group";
		shutdownCameras();
		return false;
	}

	for (int i = 0; i < cameraCount; i++)
	{
		int cameraId = cameraData[camera[i]->Serial()].first;
		bool cameraCapture = cameraData[camera[i]->Serial()].second;
		camera[i]->SetNumeric(true, cameraId);
		camera[i]->SetVideoType(Core::eVideoMode::MJPEGMode);
		camera[i]->SetMJPEGQuality(100);
		camera[i]->SetFrameRate(cameraFps);
		camera[i]->SetLateDecompression(false);

		if (cameraCapture)
		{
			cameraCountActive++;
			sync->AddCamera(camera[i]);
		}
		
		camera[i]->Start();
	}

	return true;
}

Packet* OptitrackCamera::getPacket()
{
	CameraLibrary::FrameGroup* frameGroup = sync->GetFrameGroup();
	
	if (frameGroup)
	{
		if (frameGroup->Count() != cameraCountActive)
		{			
			frameGroup->Release();
			BOOST_LOG_TRIVIAL(warning) << "Dropped unsynced frame";
			return nullptr;
		}

		Packet* packet = new Packet();

		for (int i = 0; i < frameGroup->Count(); i++)
		{
			CameraLibrary::Frame* frame = frameGroup->GetFrame(i);
			CameraLibrary::Camera* camera = frame->GetCamera();

			int cameraId = cameraData[camera->Serial()].first;
			int cameraWidth = camera->Width();
			int cameraHeight = camera->Height();

			cv::Mat frameMat = cv::Mat(cv::Size(cameraWidth, cameraHeight), CV_8UC1);
			frame->Rasterize(cameraWidth, cameraHeight, (unsigned int)frameMat.step, 8, frameMat.data);

			packet->addData(cameraId, frameMat);
			frame->Release();
		}

		frameGroup->Release();
		return packet;
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
	CameraLibrary::cModuleSync::Destroy(sync);

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

	CameraLibrary::CameraManager::X().Shutdown();
}
