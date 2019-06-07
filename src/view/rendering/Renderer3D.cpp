#include "Renderer3D.h"

Renderer3D::Renderer3D(ConfigController* configController)
{
	this->renderer = vtkSmartPointer<vtkRenderer>::New();	
	this->totalSquares = 20;
	this->squareLength = 800.0;
	this->guiFps = configController->getGuiFps();
}

void Renderer3D::render(Video3D* video3D)
{
	vtkSmartPointer<Renderer3DTimerCallback> timerCallback = vtkSmartPointer<Renderer3DTimerCallback>::New();
	timerCallback->SetVariables(video3D, renderer);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetFullScreen(true);

	vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	windowInteractor->SetRenderWindow(renderWindow);	
	windowInteractor->Initialize();
	windowInteractor->CreateRepeatingTimer(1000.0 / guiFps);

	vtkSmartPointer<Renderer3DKeypressCallback> style = vtkSmartPointer<Renderer3DKeypressCallback>::New();
	windowInteractor->AddObserver(vtkCommand::TimerEvent, timerCallback);	
	windowInteractor->SetInteractorStyle(style);
	style->SetVariables(video3D, windowInteractor);

	renderBackground();
	renderGridActor();
	renderAxesActor();
	renderTextHelpActor(renderWindow);
	renderCameraActors(video3D);
	renderer->ResetCamera();
	
	windowInteractor->Start();
}

void Renderer3D::renderBackground()
{
	renderer->SetUseFXAA(false);
	renderer->GradientBackgroundOn();
	renderer->SetBackground(25 / 255.0, 25 / 255.0, 25 / 255.0);
	renderer->SetBackground2(50 / 255.0, 50 / 255.0, 50 / 255.0);
}

void Renderer3D::renderGridActor()
{
	vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	double halfPlaneLength = totalSquares * squareLength / 2;

	for (int row = 0; row < totalSquares; row++)
	{
		double origin[3] = { row * squareLength - halfPlaneLength, -halfPlaneLength, 0.0 };
		double end[3] = { row * squareLength - halfPlaneLength, (totalSquares - 1) * squareLength - halfPlaneLength, 0.0 };

		points->InsertNextPoint(origin);
		points->InsertNextPoint(end);
	}

	for (int col = 0; col < totalSquares; col++)
	{
		double origin[3] = { -halfPlaneLength, col * squareLength - halfPlaneLength, 0.0 };
		double end[3] = { (totalSquares - 1) * squareLength - halfPlaneLength, col * squareLength - halfPlaneLength, 0.0 };

		points->InsertNextPoint(origin);
		points->InsertNextPoint(end);
	}

	linesPolyData->SetPoints(points);
	vtkSmartPointer<vtkCellArray> gridLines = vtkSmartPointer<vtkCellArray>::New();

	for (int pointIndex = 0; pointIndex <= totalSquares * 4; pointIndex += 2)
	{
		vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
		line->GetPointIds()->SetId(0, pointIndex);
		line->GetPointIds()->SetId(1, pointIndex + 1);

		gridLines->InsertNextCell(line);
	}

	linesPolyData->SetLines(gridLines);

	vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	unsigned char gridLineColor[3] = { 100, 100, 100 };

	for (int pointIndex = 0; pointIndex < totalSquares * 2; pointIndex++)
	{
		colors->InsertNextTypedTuple(gridLineColor);
	}

	linesPolyData->GetCellData()->SetScalars(colors);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(linesPolyData);

	vtkSmartPointer<vtkActor> gridActor = vtkSmartPointer<vtkActor>::New();
	gridActor->SetMapper(mapper);

	renderer->AddActor(gridActor);
}

void Renderer3D::renderAxesActor()
{
	vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	int halfPlane = squareLength * totalSquares / 2;
	double axisBegin[3] = { 0.0, 0.0, 0.0 };
	double xAxisEnd[3] = { squareLength, 0.0, 0.0 };
	double yAxisEnd[3] = { 0.0, squareLength, 0.0 };
	double zAxisEnd[3] = { 0.0, 0.0, squareLength };

	points->InsertNextPoint(axisBegin);
	points->InsertNextPoint(xAxisEnd);
	points->InsertNextPoint(yAxisEnd);
	points->InsertNextPoint(zAxisEnd);

	linesPolyData->SetPoints(points);

	vtkSmartPointer<vtkCellArray> axisLines = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkLine> xLine = vtkSmartPointer<vtkLine>::New();
	xLine->GetPointIds()->SetId(0, 0);
	xLine->GetPointIds()->SetId(1, 1);
	axisLines->InsertNextCell(xLine);

	vtkSmartPointer<vtkLine> yLine = vtkSmartPointer<vtkLine>::New();
	yLine->GetPointIds()->SetId(0, 0);
	yLine->GetPointIds()->SetId(1, 2);
	axisLines->InsertNextCell(yLine);

	vtkSmartPointer<vtkLine> zLine = vtkSmartPointer<vtkLine>::New();
	zLine->GetPointIds()->SetId(0, 0);
	zLine->GetPointIds()->SetId(1, 3);
	axisLines->InsertNextCell(zLine);

	linesPolyData->SetLines(axisLines);

	vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);

	unsigned char xAxisColor[3] = { 235, 85, 60 };
	unsigned char yAxisColor[3] = { 60, 175, 105 };
	unsigned char zAxisColor[3] = { 30, 100, 155 };

	colors->InsertNextTypedTuple(xAxisColor);
	colors->InsertNextTypedTuple(yAxisColor);
	colors->InsertNextTypedTuple(zAxisColor);

	linesPolyData->GetCellData()->SetScalars(colors);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(linesPolyData);

	vtkSmartPointer<vtkActor> axisActor = vtkSmartPointer<vtkActor>::New();
	axisActor->SetMapper(mapper);
	axisActor->GetProperty()->SetLineWidth(2);

	renderTextActor("X", cv::Point3d(squareLength * 1.25, 0.0, 0.0));
	renderTextActor("Y", cv::Point3d(0.0, squareLength * 1.25, 0.0));
	renderTextActor("Z", cv::Point3d(0.0, 0.0, squareLength * 1.25));

	renderer->AddActor(axisActor);
}

void Renderer3D::renderTextActor(std::string text, cv::Point3d position)
{
	double scale = 200.0;

	vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();
	textSource->SetText(text.c_str());
	textSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(textSource->GetOutputPort());

	vtkSmartPointer<vtkFollower> textActor = vtkSmartPointer<vtkFollower>::New();
	textActor->SetMapper(mapper);
	textActor->SetPosition(position.x, position.y, position.z);
	textActor->SetScale(scale);
	textActor->SetCamera(renderer->GetActiveCamera());	

	renderer->AddActor(textActor);
}

void Renderer3D::renderTextHelpActor(vtkSmartPointer<vtkRenderWindow> renderWindow)
{
	int screenWidth = renderWindow->GetSize()[0];
	int screenHeight = renderWindow->GetSize()[1];

	vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetDisplayPosition(10, screenHeight - 45);
	textActor->SetInput("Press 'ESC' to close 3D visualization\nPress 'Enter' to toggle animation");
	textActor->GetTextProperty()->SetFontSize(14);
	textActor->GetTextProperty()->SetLineSpacing(1.5);
	textActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
	textActor->GetTextProperty()->SetShadow(true);
	textActor->GetTextProperty()->SetFontFamily(VTK_FONT_FILE);
	textActor->GetTextProperty()->SetFontFile("./font/Roboto-Regular.ttf");
	
	renderer->AddActor2D(textActor);
}

void Renderer3D::renderCameraActors(Video3D* video3D)
{
	double scale = 400.0;

	std::map<int, Intrinsics*> intrinsics = video3D->getIntrinsics();
	std::map<int, Extrinsics*> extrinsics = video3D->getExtrinsics();

	for (int cameraNumber : video3D->getCameras())
	{
		cv::Mat cameraMatrix = intrinsics[cameraNumber]->getCameraMatrix();

		double fX = cameraMatrix.at<double>(0, 0);
		double fY = cameraMatrix.at<double>(1, 1);
		double cY = cameraMatrix.at<double>(1, 2);
		double fovY = 2.0 * atan2(cY, fY) * 180 / CV_PI;
		double aspectRatio = fY / fX;

		vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
		camera->SetViewAngle(fovY);
		camera->SetPosition(0.0, 0.0, 0.0);
		camera->SetViewUp(0.0, 1.0, 0);
		camera->SetFocalPoint(0.0, 0.0, 1.0);
		camera->SetClippingRange(1e-9, scale);

		double planesArray[24];
		camera->GetFrustumPlanes(aspectRatio, planesArray);

		vtkSmartPointer<vtkPlanes> planes = vtkSmartPointer<vtkPlanes>::New();
		planes->SetFrustumPlanes(planesArray);

		vtkSmartPointer<vtkFrustumSource> frustumSource = vtkSmartPointer<vtkFrustumSource>::New();
		frustumSource->SetPlanes(planes);

		vtkSmartPointer<vtkExtractEdges> extractEdges = vtkSmartPointer<vtkExtractEdges>::New();
		extractEdges->SetInputConnection(frustumSource->GetOutputPort());
		extractEdges->Update();

		vtkSmartPointer<vtkPolyData> polydata = extractEdges->GetOutput();
		int polyDataSize = polydata->GetNumberOfPoints();

		cv::Vec3d color;
		double reprojectionError = extrinsics[cameraNumber]->getReprojectionError();
		if (reprojectionError > 1.0)
		{
			color[0] = 235;
			color[1] = 85;
			color[2] = 60;
		}
		else if (reprojectionError > 0.5)
		{
			color[0] = 235;
			color[1] = 190;
			color[2] = 60;
		}
		else
		{
			color[0] = 60;
			color[1] = 175;
			color[2] = 105;
		}

		cv::Vec3b rgb = cv::Vec3d(color[0], color[1], color[2]);
		cv::Vec3b* color_data = new cv::Vec3b[polyDataSize];
		std::fill(color_data, color_data + polyDataSize, rgb);

		vtkSmartPointer<vtkUnsignedCharArray> scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
		scalars->SetName("Colors");
		scalars->SetNumberOfComponents(3);
		scalars->SetNumberOfTuples((vtkIdType)polyDataSize);
		scalars->SetArray(color_data->val, (vtkIdType)(polyDataSize * 3), 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);
		polydata->GetPointData()->SetScalars(scalars);

		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(polydata);

		vtkSmartPointer<vtkActor> cameraActor = vtkSmartPointer<vtkActor>::New();
		cameraActor->SetMapper(mapper);

		cv::Mat rotationMatrix;
		cv::Mat rotationVector = extrinsics[cameraNumber]->getRotationVector();
		cv::Rodrigues(rotationVector, rotationMatrix);
		cv::Mat translationVector = extrinsics[cameraNumber]->getTranslationVector();

		cv::Mat cameraRotatationMatrix = rotationMatrix.t();
		cv::Mat cameraTranslationVector = -rotationMatrix.t() * translationVector;

		cv::Affine3d cameraTransform = cv::Affine3d(cameraRotatationMatrix, cameraTranslationVector);
		transformActor(cameraActor, cameraTransform);

		renderer->AddActor(cameraActor);

		cv::Mat cameraRotationVector;
		cv::Rodrigues(cameraRotatationMatrix, cameraRotationVector);

		cv::Mat lookAt = rotationMatrix.row(2).t();
		cv::Point3d cameraTextPosition = cv::Point3d(cv::Mat(cameraTranslationVector - lookAt * scale));
		renderTextActor(std::to_string(cameraNumber), cv::Point3d(cameraTextPosition.x, cameraTextPosition.y, cameraTextPosition.z));
	}
}

void Renderer3D::transformActor(vtkSmartPointer<vtkActor> actor, cv::Affine3d transform)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	matrix->DeepCopy(transform.matrix.val);
	actor->SetUserMatrix(matrix);
	actor->Modified();
}

Renderer3DTimerCallback* Renderer3DTimerCallback::New()
{
	Renderer3DTimerCallback* callbackObject = new Renderer3DTimerCallback();
	return callbackObject;
}

void Renderer3DTimerCallback::SetVariables(Video3D* video3D, vtkSmartPointer<vtkRenderer> renderer)
{
	this->video3D = video3D;
	this->renderer = renderer;
	this->previousActors = std::list<vtkSmartPointer<vtkActor>>();
	this->cornerText = getTextActor(cv::Point2i(10, 10));
	renderer->AddActor2D(cornerText);
}

vtkSmartPointer<vtkTextActor> Renderer3DTimerCallback::getTextActor(cv::Point2i position)
{
	vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetDisplayPosition(position.x, position.y);
	textActor->GetTextProperty()->SetFontSize(14);
	textActor->GetTextProperty()->SetLineSpacing(1.5);
	textActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
	textActor->GetTextProperty()->SetShadow(true);	
	textActor->GetTextProperty()->SetFontFamily(VTK_FONT_FILE);
	textActor->GetTextProperty()->SetFontFile("./font/Roboto-Regular.ttf");
	return textActor;
}

void Renderer3DTimerCallback::Execute(vtkObject* caller, unsigned long eventId, void* vtkNotUsed(callData))
{
	vtkRenderWindowInteractor* renderWindow = dynamic_cast<vtkRenderWindowInteractor*>(caller);

	int fps = (int)(1.0 / renderer->GetLastRenderTimeInSeconds());
	int frameNumber = video3D->getFrameNumber();
	std::string labelText = "FPS: " + std::to_string(fps) + "\n" + "Frame number : " + std::to_string(frameNumber);
	cornerText->SetInput(labelText.c_str());

	if (!video3D->isVideoActive())
	{
		renderWindow->GetRenderWindow()->Render();
		return;
	}

	for (vtkSmartPointer<vtkActor> pointCloudActor : previousActors)
	{
		renderer->RemoveActor(pointCloudActor);
	}

	Frame3D* currentFrame3D = video3D->getNextFrame();

	for (int cameraNumber : video3D->getCameras())
	{
		if (currentFrame3D != nullptr)
		{
			for (std::pair<int, std::list<cv::Point3d>> frameData : currentFrame3D->getData())
			{
				int cameraNumber = frameData.first;

				vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				for (cv::Point3d point : frameData.second)
				{
					points->InsertNextPoint(point.x, point.y, point.z);
				}

				vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
				pointsPolydata->SetPoints(points);

				vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
				vertexFilter->SetInputData(pointsPolydata);
				vertexFilter->Update();

				vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
				polydata->ShallowCopy(vertexFilter->GetOutput());

				vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapper->SetInputData(polydata);

				vtkSmartPointer<vtkActor> pointCloudActor = vtkSmartPointer<vtkActor>::New();
				pointCloudActor->SetMapper(mapper);
				pointCloudActor->GetProperty()->SetPointSize(3);

				renderer->AddActor(pointCloudActor);

				previousActors.push_back(pointCloudActor);
			}
		}
	}

	renderWindow->GetRenderWindow()->Render();	
}

Renderer3DKeypressCallback* Renderer3DKeypressCallback::New()
{
	Renderer3DKeypressCallback* callbackObject = new Renderer3DKeypressCallback();
	return callbackObject;
}

void Renderer3DKeypressCallback::SetVariables(Video3D* video3D, vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor)
{
	this->video3D = video3D;
	this->windowInteractor = windowInteractor;
}

void Renderer3DKeypressCallback::OnKeyPress()
{
	vtkRenderWindowInteractor* rwi = this->Interactor;
	std::string key = rwi->GetKeySym();

	if (key == "Return")
	{
		video3D->toggleVideo();
	}
	else if (key == "Escape")
	{
		windowInteractor->SetRenderWindow(nullptr);
		windowInteractor->TerminateApp();
	}

	vtkInteractorStyleTrackballCamera::OnKeyPress();
}