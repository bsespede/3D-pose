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
	renderBackground();
	renderGridActor();
	renderAxesActor();
	renderCameraActors(video3D);
	renderer->ResetCamera();

	vtkSmartPointer<Renderer3DCallback> timerCallback = vtkSmartPointer<Renderer3DCallback>::New();
	timerCallback->initialize(video3D, renderer);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(renderWindow->GetScreenSize()[0] - 50, renderWindow->GetScreenSize()[1] - 100);

	vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	windowInteractor->SetRenderWindow(renderWindow);
	windowInteractor->Initialize();
	windowInteractor->CreateRepeatingTimer(1000.0 / guiFps);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	windowInteractor->AddObserver(vtkCommand::TimerEvent, timerCallback);
	windowInteractor->SetInteractorStyle(style);
	windowInteractor->Start();
}

void Renderer3D::renderBackground()
{
	renderer->SetUseFXAA(true);
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

	unsigned char xAxisColor[3] = { 237, 85, 59 };
	unsigned char yAxisColor[3] = { 60, 174, 106 };
	unsigned char zAxisColor[3] = { 32, 99, 155 };

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

		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(polydata);

		vtkSmartPointer<vtkActor> cameraActor = vtkSmartPointer<vtkActor>::New();
		cameraActor->SetMapper(mapper);

		cv::Mat rotationMatrix;
		cv::Mat rotationVector = extrinsics[cameraNumber]->getRotationVector();
		cv::Rodrigues(rotationVector, rotationMatrix);
		cv::Mat translationVector = extrinsics[cameraNumber]->getTranslationVector();

		cv::Mat cameraRotatationVector = rotationMatrix.t();
		cv::Mat cameraTranslationVector = -rotationMatrix.t() * translationVector;

		cv::Affine3d cameraTransform = cv::Affine3d(cameraRotatationVector, cameraTranslationVector);
		transformActor(cameraActor, cameraTransform);

		renderer->AddActor(cameraActor);

		cv::Point3d cameraTextPosition = cv::Point3d(cameraTranslationVector);
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

Renderer3DCallback* Renderer3DCallback::New()
{
	Renderer3DCallback* callbackObject = new Renderer3DCallback;

	return callbackObject;
}

void Renderer3DCallback::initialize(Video3D* video3D, vtkSmartPointer<vtkRenderer> renderer)
{
	this->video3D = video3D;
	this->renderer = renderer;
	this->fpsText = getTextActor(cv::Point2i(10, 30));
	this->frameNumberText = getTextActor(cv::Point2i(10, 10));

	renderer->AddActor2D(fpsText);
	renderer->AddActor2D(frameNumberText);
}

vtkSmartPointer<vtkTextActor> Renderer3DCallback::getTextActor(cv::Point2i position)
{
	vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetInput("");
	textActor->SetDisplayPosition(position.x, position.y);
	textActor->GetTextProperty()->SetFontSize(14);
	textActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
	
	return textActor;
}

void Renderer3DCallback::Execute(vtkObject* caller, unsigned long eventId, void* vtkNotUsed(callData))
{
	vtkRenderWindowInteractor* renderWindow = dynamic_cast<vtkRenderWindowInteractor*>(caller);
	Frame3D* currentFrame3D = video3D->getNextFrame();
	std::list<vtkSmartPointer<vtkActor>> pointCloudActors;

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
				pointCloudActor->GetProperty()->SetPointSize(5);

				renderer->AddActor(pointCloudActor);

				pointCloudActors.push_back(pointCloudActor);
			}
		}
	}

	int fps = (int)(1.0 / renderer->GetLastRenderTimeInSeconds());
	fpsText->SetInput(("FPS: " + std::to_string(fps)).c_str());

	int frameNumber = video3D->getFrameNumber();
	frameNumberText->SetInput(("Frame number: " + std::to_string(frameNumber)).c_str());

	renderWindow->GetRenderWindow()->Render();

	for (vtkSmartPointer<vtkActor> pointCloudActor : pointCloudActors)
	{
		renderer->RemoveActor(pointCloudActor);
	}
}