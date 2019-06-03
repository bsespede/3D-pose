cmake_minimum_required(VERSION 3.9)
project(OpticalMocap LANGUAGES CXX)

# Find Qt package and set MOC and RCC vars
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include OpenMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

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
								"view/enum/Input.h"
								"view/Console.cpp" "view/Console.h"
								"view/rendering/Renderer2D.cpp" "view/rendering/Renderer2D.h"
								"view/rendering/Renderer3D.cpp" "view/rendering/Renderer3D.h"
								"model/AppController.cpp" "model/AppController.h"		
								"model/enum/CaptureType.h" "model/enum/CalibrationType.h"
								"model/scene/Scene.cpp" "model/scene/Scene.h"
								"model/scene/SceneController.cpp" "model/scene/SceneController.h"
								"model/camera/capture/Capture.cpp" "model/camera/capture/Capture.h"								
								"model/camera/capture/Packet.cpp" "model/camera/capture/Packet.h"
								"model/camera/optitrack/OptitrackCamera.cpp" "model/camera/optitrack/OptitrackCamera.h"
								"model/camera/CameraController.cpp" "model/camera/CameraController.h"
								"model/config/ConfigController.cpp" "model/config/ConfigController.h"
 								"model/calibration/parameters/Intrinsics.cpp" "model/calibration/parameters/Intrinsics.h"
								"model/calibration/parameters/Extrinsics.cpp" "model/calibration/parameters/Extrinsics.h"								
								"model/calibration/CalibrationController.cpp" "model/calibration/CalibrationController.h")

# Link it all together
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} Boost::boost OptiTrack::CameraLibrary2015x64S)