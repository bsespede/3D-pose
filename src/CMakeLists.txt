cmake_minimum_required(VERSION 3.9)
project(OpticalMocap LANGUAGES CXX)

# Find Qt package and set MOC and RCC vars
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include OpenMP
include(FindOpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# Include OptiTrack headers, libraries
set(OptiTrack_INCLUDE_DIR "$ENV{OptiTrack_DIR}/include")
include_directories(${OptiTrack_INCLUDE_DIR})
set(OptiTrack_LIB_DIR "$ENV{OptiTrack_DIR}/lib")
add_library(OptiTrack::CameraLibrary2015x64S STATIC IMPORTED)
set_property(TARGET OptiTrack::CameraLibrary2015x64S PROPERTY IMPORTED_LOCATION ${OptiTrack_LIB_DIR}/CameraLibrary2015x64S.lib)

#Include OpenCV package, headers, libraries
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

#Include Boost package, headers, libraries
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "Main.cpp" 
								"view/Console.cpp" "view/Console.h"
								"view/CameraRenderer.cpp" "view/CameraRenderer.h"
								"view/enum/Input.h"
								"model/scene/Scene.cpp" "model/scene/Scene.h" 
								"model/scene/enum/Operation.h"
								"model/camera/FramesPacket.cpp" "model/camera/FramesPacket.h"
								"model/calibration/IntrinsicCalibration.cpp" "model/calibration/IntrinsicCalibration.h"
								"model/calibration/ExtrinsicCalibration.cpp" "model/calibration/ExtrinsicCalibration.h"
								"model/capture/Capture.cpp" "model/capture/Capture.h"
								"model/camera/enum/CaptureMode.h"
								"controller/AppController.cpp" "controller/AppController.h"
								"controller/files/FileController.cpp" "controller/files/FileController.h"
								"controller/camera/CameraController.cpp" "controller/camera/CameraController.h"
								"controller/calibration/CalibrationController.cpp" "controller/calibration/CalibrationController.h"
								"controller/camera/optitrack/OptitrackCamera.cpp" "controller/camera/optitrack/OptitrackCamera.h")

# Link it all together
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} Boost::boost OptiTrack::CameraLibrary2015x64S)