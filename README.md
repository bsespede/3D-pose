# 3DPose

![3DPose](https://raw.githubusercontent.com/bsespede/3D-pose/master/summary.png "Sample visualization of the calibration process")

## Introduction

This repository contains a multi-view optical motion capture system. It includes a multi-camera calibration tool with useful visualizations for validation.

It is composed by the following modules:
- Capture module:
	- Sinchronized capture using Optitrack cameras (but can easily be extended to any type of camera).
- Calibration:
	- For intrinsics + board pose estimatio [1][2]
	- For extrinsics + bundle adjustment [3]
	- Dynamic 3D visualization of calibration board
- 3D human pose reconstruction:
	- OpenPose 3D module with CERES optimization [4]

## Libraries

The following libraries were used:
- OpenCV 4.1.0 + Contrib Modules (arcuo)
- OptiTrack Camera SDK 2.1.1
- OpenPose 1.5
- VTK 8.4
- Boost 1.67.0

## References

[1] S. Garrido-Jurado, R. Muñoz-Salinas, F. J. Madrid-Cuevas, and M. J. Marín-Jiménez. 2014. "Automatic generation and detection of highly reliable fiducial markers under occlusion", PR, 2014.

[2] Z. Zhang, "A flexible new technique for camera calibration," TPAMI, 2000.

[3] C. Wu, S. Agarwal, B. Curless and S. M. Seitz, "Multicore bundle adjustment," CVPR, 2011.

[4] C. Zhe, S. Tomas, W. Shih-En, S. Yaser, "Realtime Multi-Person 2D Pose Estimation using Part Affinity Fields", CVPR, 2017. 
