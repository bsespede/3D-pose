#include "OptitrackCamera.h"

OptitrackCamera::OptitrackCamera() : cameraCount(0)
{
	CameraLibrary_EnableDevelopment();
}

bool OptitrackCamera::startCameras(Core::eVideoMode mode)
{
	CameraLibrary::CameraManager::X();
	CameraManager::X().WaitForInitialization();
	cameraCount = 0;

	for (int i = 0; i < list.Count(); i++)
	{
		camera[i] = CameraManager::X().GetCamera(list[i].UID());

		if (camera[i] == nullptr)
		{
			BOOST_LOG_TRIVIAL(warning) << "Couldn't connect to camera " + std::to_string(i);
		}
		else
		{
			cameraCount++;
		}
	}

	if (cameraCount == 0)
	{
		BOOST_LOG_TRIVIAL(error) << "Couldn't connect to any cameras";
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
		camera[i]->Start();
		camera[i]->SetVideoType(mode);
	}

	return true;
}

FramesPacket* OptitrackCamera::captureFramesPacket()
{
	FrameGroup* frameGroup = sync->GetFrameGroup();
	FramesPacket* framesPacket = new FramesPacket();

	if (frameGroup)
	{
		for (int i = 0; i < frameGroup->Count(); i++)
		{
			Frame* frame = frameGroup->GetFrame(i);
			Camera* camera = frame->GetCamera();

			int cameraId = camera->Serial();
			int cameraWidth = camera->Width();
			int cameraHeight = camera->Height();

			cv::Mat frameMat = cv::Mat(cv::Size(cameraWidth, cameraHeight), CV_8UC1);
			frame->Rasterize(cameraWidth, cameraHeight, frameMat.step, 8, frameMat.data);

			framesPacket->addFrame(cameraId, frameMat);
			frame->Release();
		}

		frameGroup->Release();
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Error during synchronized capture";
	}

	return framesPacket;
}

void OptitrackCamera::stopCameras()
{
	sync->RemoveAllCameras();
	cModuleSync::Destroy(sync);

	for (int i = 0; i < cameraCount; i++)
	{
		camera[i]->Stop();
	}

	for (int i = 0; i < cameraCount; i++)
	{
		camera[i]->Release();
	}

	CameraManager::X().Shutdown();
}
