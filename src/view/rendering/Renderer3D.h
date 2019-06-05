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
#include "model/video/Video3D.h"
#include "model/config/ConfigController.h"

class Renderer3D
{
public:
	Renderer3D(ConfigController* configController);
	void render(Video3D* video3D);
private:
	vtkSmartPointer<vtkActor> getGridActor();
	vtkSmartPointer<vtkActor> getAxesActor();
	vtkSmartPointer<vtkFollower> getTextActor(std::string text, cv::Point3d position, vtkSmartPointer<vtkCamera> cameraToFollow);
	std::vector<vtkSmartPointer<vtkActor>> getCameraActors(Video3D* video3D);
	std::vector<vtkSmartPointer<vtkActor>> getCloudActors(Video3D* video3D);
	void transformActor(vtkSmartPointer<vtkActor> actor, cv::Affine3d transform);
	void updateCloudActors(Frame3D* previousFrame, Frame3D* currentFrame, std::vector<vtkSmartPointer<vtkActor>> cloudActors);
	int guiFps;
	int totalSquares;
	double squareLength;
};
