#include "OptitrackCamera.h"

OptitrackCamera::OptitrackCamera()
{
	CameraLibrary_EnableDevelopment();
}

bool OptitrackCamera::startCameras(Core::eVideoMode mode)
{
	CameraManager::X();
	CameraManager::X().WaitForInitialization();
	cameraCount = 0;

	for (int i = 0; i < list.Count(); i++)
	{
		camera[i] = CameraManager::X().GetCamera(list[i].UID());

		if (camera[i] == nullptr)
		{
			BOOST_LOG_TRIVIAL(warning) << "Couldn't connect to camera";
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
		camera[i]->Start();
		camera[i]->SetVideoType(mode);
	}

	return true;
}

FramesPacket* OptitrackCamera::captureFramesPacket()
{
	FrameGroup* frameGroup = sync->GetFrameGroup();
	
	if (frameGroup)
	{
		FramesPacket* framesPacket = new FramesPacket();

		for (int i = 0; i < frameGroup->Count(); i++)
		{
			Frame* frame = frameGroup->GetFrame(i);
			Camera* camera = frame->GetCamera();

			int cameraId = camera->CameraID();
			int cameraWidth = camera->Width();
			int cameraHeight = camera->Height();

			cv::Mat frameMat = cv::Mat(cv::Size(cameraWidth, cameraHeight), CV_8UC1);
			frame->Rasterize(cameraWidth, cameraHeight, frameMat.step, 8, frameMat.data);

			framesPacket->addFrame(cameraId, frameMat);
			frame->Release();
		}

		frameGroup->Release();
		return framesPacket;
	}

	return nullptr;
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
