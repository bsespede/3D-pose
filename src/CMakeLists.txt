cmake_minimum_required(VERSION 3.9)
project(3DPose LANGUAGES CXX)

# Include OpenMP
include(FindOpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# Find Qt package and set MOC and RCC vars
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Quick Widgets QuickWidgets QuickControls2 REQUIRED)

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

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "Main.cpp" 
								"frontend/Console.cpp" "frontend/Console.h"
								"backend/scene/SceneManager.cpp" "backend/scene/SceneManager.h"
								"backend/scene/Scene.cpp" "backend/scene/Scene.h"
								"backend/camera/OptitrackCamera.cpp" "backend/camera/OptitrackCamera.h"
								"backend/scene/enum/CaptureMode.h")

# Link it all together
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} OptiTrack::CameraLibrary2015x64S Qt5::Widgets Qt5::Core Qt5::Quick Qt5::QuickWidgets Qt5::QuickControls2)