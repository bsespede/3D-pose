# OpticalMocap

## Introduction

This repository contains a multi-view optical motion capture system.

It is composed by the following modules:
- Capture module (sinchronization of cameras and recording)
- Calibration (point based multi-view self-calibration + bundle adjustment)
- 3D pose reconstruction:
	- Automatic classification and tracking of points of interest using DeepLabCut
	- 3D reconstruction of points of interest
	- Kalman filter for temporal occlusion solving
- FUTURE: Exporting to BVH file format

## Libraries

The following libraries were used:
- OpenCV 4.0.1
- OptiTrack Camera SDK 2.1.1