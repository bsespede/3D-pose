#pragma once

#include <map>
#include <chrono>
#include <thread>
#include <vtkFrustumSource.h>
#include <vtkExtractEdges.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkCamera.h>
#include <vtkPlanes.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkCallbackCommand.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkPointData.h>
#include <vtkLineSource.h>
#include <vtkInteractorStyleJoystickCamera.h>
#include <vtkCullerCollection.h>
#include <chrono>
#include "model/video/Video3D.h"
#include "model/config/ConfigController.h"

class Renderer3D
{
public:
	Renderer3D(ConfigController* configController);
	void render(Video3D* video3D);
private:
	void renderBackground();
	void renderGridActor();
	void renderAxesActor();
	void renderTextHelpActor(int screenHeight);
	void renderTextActor(std::string text, cv::Point3d position);
	void renderCameraActors(Video3D* video3D);
	void transformActor(vtkSmartPointer<vtkActor> actor, cv::Affine3d transform);
	void resetActiveCamera();
	vtkSmartPointer<vtkRenderer> renderer;	
	int guiFps;
	int totalSquares;
	double squareLength;	
};

class Renderer3DTimerCallback : public vtkCallbackCommand
{
public:	
	static Renderer3DTimerCallback* New();
	void SetVariables(Video3D* video3D, vtkSmartPointer<vtkRenderer> renderer);
	virtual void Execute(vtkObject* caller, unsigned long eventId, void* vtkNotUsed(callData));
private:
	std::vector<vtkSmartPointer<vtkActor>> previousActors;
	vtkSmartPointer<vtkTextActor> getTextActor(cv::Point2i position);
	vtkSmartPointer<vtkTextActor> cornerText;	
	vtkSmartPointer<vtkRenderer> renderer;
	Video3D* video3D;
};

class Renderer3DKeypressCallback : public vtkCallbackCommand
{
public:
	static Renderer3DKeypressCallback* New();
	void SetVariables(Video3D* video3D);
	virtual void Execute(vtkObject* caller, unsigned long eventId, void* vtkNotUsed(callData));
private:
	Video3D* video3D;
};