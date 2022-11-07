cmake_minimum_required(VERSION 3.21)
project(Maxform)
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(
	SOURCE_FILES
	"pluginMain.cpp"
	"Maxformations.h"
	"Maxformations.cpp"
	"Matrix3.cpp"
	"Matrix3.h"
	"Maxform.cpp"
	"Maxform.h"
	"ExposeTransform.cpp"
	"ExposeTransform.h"
	"Matrix3Controller.cpp"
	"Matrix3Controller.h"
	"PRS.cpp"
	"PRS.h"
	"IKChainControl.cpp"
	"IKChainControl.h"
	"IKControl.cpp"
	"IKControl.h"
	"PositionList.cpp"
	"PositionList.h"
	"RotationList.cpp"
	"RotationList.h"
	"ScaleList.cpp"
	"ScaleList.h"
)

set(
	LIBRARIES
	OpenMaya
	OpenMayaAnim
	Foundation
)

build_plugin()