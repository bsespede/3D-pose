# OpticalMocap

## Introduction

This repository contains a multi-view optical motion capture system.

It is composed by the following modules:
- Capture module:
	- Sinchronized capture using Optitrack cameras (but can easily be extended to any type of camera).
- Calibration:
	- For intrinsics + distortion: Charuco [REF]
	- For extrinsics: Nister-5-points [REF] + GPU Bundle-adjustment [REF]
- 3D human pose reconstruction:
	- OpenPose + CERES optimization [REF]

## Libraries

The following libraries were used:
- OpenCV 4.1.0 + Contrib Modules 
- OptiTrack Camera SDK 2.1.1
- OpenPose 1.5

## References

__TODO__