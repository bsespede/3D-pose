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
#include <vtkPlaneSource.h>
#include <vtkTextureMapToPlane.h>
#include <vtkImageData.h>
#include <vtkAppendPolyData.h>
#include <vtkTexture.h>
#include <vtkImageData.h>
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
	void renderTextActor(std::string text, cv::Point3d position);
	void renderCameraActors(Video3D* video3D);
	void transformActor(vtkSmartPointer<vtkActor> actor, cv::Affine3d transform);	
	vtkSmartPointer<vtkRenderer> renderer;	
	int guiFps;
	int totalSquares;
	double squareLength;	
};

class Renderer3DCallback : public vtkCommand
{
public:	
	static Renderer3DCallback* New();
	void initialize(Video3D* video3D, vtkSmartPointer<vtkRenderer> renderer);
	virtual void Execute(vtkObject* caller, unsigned long eventId, void* vtkNotUsed(callData));
private:
	vtkSmartPointer<vtkTextActor> getTextActor(cv::Point2i position);
	vtkSmartPointer<vtkTextActor> frameNumberText;
	vtkSmartPointer<vtkTextActor> fpsText;	
	vtkSmartPointer<vtkRenderer> renderer;
	Video3D* video3D;
};