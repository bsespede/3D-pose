# 3DPose

## Introduction

This repository contains a multi-view optical motion capture system with support 3D visualizations. It includes a multi-camera calibration tool.

It is composed by the following modules:
- **Capture module**:
	- Sinchronized capture using Optitrack cameras (but can easily be extended to any type of camera by implementing the adequate interfaces)
	- Dynamic 2D visualization of the cameras feed
- **Calibration module**:
	- For intrinsics: charuco based calibration [1][2]
	- For extrinsics: charuco board pose estimation + nister 5-point algorithm [3]
	- Dynamic 3D visualization of calibration board
- **Human pose estimation module**:
	- OpenPose 3D module with CERES non-linear optimization [4]

## Libraries

The following libraries were used:
- OpenCV 4.1.0 + Contrib Modules (aruco)
- OptiTrack Camera SDK 2.1.1
- OpenPose 1.5 (customized)
- VTK 8.2
- Boost 1.67

## References

> [1] S. Garrido-Jurado, R. Muñoz-Salinas, F. J. Madrid-Cuevas, and M. J. Marín-Jiménez, "Automatic generation and detection of highly reliable fiducial markers under occlusion", PR, 2014.

> [2] Z. Zhang, "A flexible new technique for camera calibration," TPAMI, 2000.

> [3] D. Nister, "An efficient solution to the five-point relative pose problem," TPAMI, 2004.

> [4] C. Zhe, S. Tomas, W. Shih-En, S. Yaser, "Realtime Multi-Person 2D Pose Estimation using Part Affinity Fields", CVPR, 2017. 
