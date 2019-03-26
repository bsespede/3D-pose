#include "OptitrackCamera.h"

OptitrackCamera::OptitrackCamera()
{
	CameraLibrary_EnableDevelopment();
}

bool OptitrackCamera::initialize()
{
	CameraLibrary::CameraManager::X();
	CameraLibrary::CameraManager::X().WaitForInitialization();
	cameraCount = 0;

	for (int i = 0; i < list.Count(); i++)
	{
		camera[i] = CameraLibrary::CameraManager::X().GetCamera(list[i].UID());

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
}

void OptitrackCamera::start(CaptureMode captureMode)
{
	sync = CameraLibrary::cModuleSync::Create();

	for (int i = 0; i < cameraCount; i++)
	{
		sync->AddCamera(camera[i]);
	}

	Core::eVideoMode mode; 
	
	if (captureMode == CaptureMode::UNCALIBRATED_CAPTURE)
	{
		mode = Core::eVideoMode::SegmentMode;
	}
	else
	{
		mode = Core::eVideoMode::GrayscaleMode;
	}

	for (int i = 0; i < cameraCount; i++)
	{
		camera[i]->Start();
		camera[i]->SetVideoType(mode);
	}
}

std::map<int, cv::Mat> OptitrackCamera::capture()
{
	CameraLibrary::FrameGroup* frameGroup = sync->GetFrameGroup();
	std::map<int, cv::Mat> frameMap = std::map<int, cv::Mat>();

	if (frameGroup)
	{
		for (int i = 0; i < frameGroup->Count(); i++)
		{
			CameraLibrary::Frame* frame = frameGroup->GetFrame(i);
			CameraLibrary::Camera* camera = frame->GetCamera();

			int cameraId = camera->Serial();
			int cameraWidth = camera->Width();
			int cameraHeight = camera->Height();

			cv::Mat frameMat = cv::Mat(cv::Size(cameraWidth, cameraHeight), CV_8UC1);
			frame->Rasterize(cameraWidth, cameraHeight, frameMat.step, 8, frameMat.data);

			frameMap[cameraId] = frameMat;

			frame->Release();
		}

		frameGroup->Release();
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Error during synchronized capture";
	}

	return frameMap;
}

void OptitrackCamera::stop()
{
	sync->RemoveAllCameras();
	CameraLibrary::cModuleSync::Destroy(sync);

	for (int i = 0; i < cameraCount; i++)
	{
		camera[i]->Stop();
	}
}

void OptitrackCamera::release()
{
	for (int i = 0; i < cameraCount; i++)
	{
		camera[i]->Release();
	}

	CameraLibrary::CameraManager::X().Shutdown();
}