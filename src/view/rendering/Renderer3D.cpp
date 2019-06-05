#include "Renderer3D.h"

Renderer3D::Renderer3D(ConfigController* configController)
{
	this->totalSquares = configController->getTotalSquares();
	this->squareLength = configController->getSquareLength();
	this->guiFps = configController->getGuiFps();
}

void Renderer3D::render(Video3D* video3D)
{
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();	
	renderer->SetUseFXAA(true);
	renderer->GradientBackgroundOn();
	renderer->SetBackground(25 / 255.0, 25 / 255.0, 25 / 255.0);
	renderer->SetBackground2(50 / 255.0, 50 / 255.0, 50 / 255.0);

	vtkSmartPointer<vtkActor> gridActor = getGridActor();
	renderer->AddActor(gridActor);

	vtkSmartPointer<vtkActor> axesActor = getAxesActor();
	vtkSmartPointer<vtkFollower> xAxisLabel = getTextActor("X", cv::Point3d(squareLength * 1.25, 0.0, 0.0), renderer->GetActiveCamera());
	vtkSmartPointer<vtkFollower> yAxisLabel = getTextActor("Y", cv::Point3d(0.0, squareLength * 1.25, 0.0), renderer->GetActiveCamera());
	vtkSmartPointer<vtkFollower> zAxisLabel = getTextActor("Z", cv::Point3d(0.0, 0.0, squareLength * 1.25), renderer->GetActiveCamera());
	renderer->AddActor(axesActor);
	renderer->AddActor(xAxisLabel);
	renderer->AddActor(yAxisLabel);
	renderer->AddActor(zAxisLabel);

	std::vector<vtkSmartPointer<vtkActor>> cameraActors = getCameraActors(video3D);
	for (vtkSmartPointer<vtkActor> cameraActor : cameraActors)
	{
		renderer->AddActor(cameraActor);
	}

	/*std::vector<vtkSmartPointer<vtkActor>> cloudActors = getCloudActors(video3D);
	for (vtkSmartPointer<vtkActor> cloudActor : cloudActors)
	{
		renderer->AddActor(cloudActor);
	}*/

	renderer->ResetCamera();

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(renderWindow->GetScreenSize());

	vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	windowInteractor->SetRenderWindow(renderWindow);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	windowInteractor->SetInteractorStyle(style);
	windowInteractor->Start();

	/*Frame3D* currentFrame = nullptr;
	while (renderWindow->)
	{
		Frame3D* previousFrame = currentFrame;
		currentFrame = video3D->getNextFrame();
		//updateCloudActors(previousFrame, currentFrame, cloudActors);

		renderer->ResetCameraClippingRange();
		renderWindow->Render();
	}*/
}

vtkSmartPointer<vtkActor> Renderer3D::getGridActor()
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

	return gridActor;
}

vtkSmartPointer<vtkActor> Renderer3D::getAxesActor()
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

	return axisActor;
}

vtkSmartPointer<vtkFollower> Renderer3D::getTextActor(std::string text, cv::Point3d position, vtkSmartPointer<vtkCamera> camera)
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
	textActor->SetCamera(camera);

	return textActor;
}

std::vector<vtkSmartPointer<vtkActor>> Renderer3D::getCameraActors(Video3D* video3D)
{
	double scale = 400.0;
	std::vector<vtkSmartPointer<vtkActor>> cameraActors;
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
		cv::Affine3d cameraTransform = cv::Affine3d(rotationMatrix, translationVector);
		transformActor(cameraActor, cameraTransform);

		cameraActors.push_back(cameraActor);
	}	

	return cameraActors;
}

std::vector<vtkSmartPointer<vtkActor>> Renderer3D::getCloudActors(Video3D* video3D)
{
	std::vector<vtkSmartPointer<vtkActor>> cloudActors;
	return cloudActors;
}

void Renderer3D::updateCloudActors(Frame3D* previousFrame, Frame3D* currentFrame, std::vector<vtkSmartPointer<vtkActor>> cloudActors)
{
	if (previousFrame != nullptr)
	{
		if (currentFrame == nullptr)
		{
			// clean actor
		}
		else
		{
			// update actor
		}
	}
	else
	{
		if (currentFrame != nullptr)
		{
			// add actor
		}
	}
}

void Renderer3D::transformActor(vtkSmartPointer<vtkActor> actor, cv::Affine3d transform)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	matrix->DeepCopy(transform.matrix.val);
	actor->SetUserMatrix(matrix);
	actor->Modified();
}