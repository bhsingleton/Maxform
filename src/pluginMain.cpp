//
// File: pluginMain.cpp
//
// Author: Benjamin H. Singleton
//

#include "Matrix3.h"
#include "Maxform.h"
#include "ExposeTransform.h"
#include "Matrix3Controller.h"
#include "PRS.h"
#include "IKChainControl.h"
#include "IKControl.h"
#include "PositionList.h"
#include "RotationList.h"
#include "ScaleList.h"

#include <maya/MFnPlugin.h>


MStatus initializePlugin(MObject obj) 
{

	MStatus status;

	// Register nodes
	//
	MFnPlugin plugin(obj, "Ben Singleton", "2017", "Any");

	status = plugin.registerTransform("maxform", Maxform::id, Maxform::creator, Maxform::initialize, Matrix3::creator, Matrix3::id, &Maxform::classification);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerTransform("exposeTm", ExposeTransform::id, ExposeTransform::creator, ExposeTransform::initialize, Matrix3::creator, Matrix3::id, &ExposeTransform::classification);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("matrix3Controller", Matrix3Controller::id, Matrix3Controller::creator, Matrix3Controller::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("prs", PRS::id, PRS::creator, PRS::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("ikChainControl", IKChainControl::id, IKChainControl::creator, IKChainControl::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("ikControl", IKControl::id, IKControl::creator, IKControl::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("positionList", PositionList::id, PositionList::creator, PositionList::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("rotationList", RotationList::id, RotationList::creator, RotationList::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("scaleList", ScaleList::id, ScaleList::creator, ScaleList::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

}

MStatus uninitializePlugin(MObject obj) 
{

	MStatus status;

	// Deregister nodes
	//
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(Maxform::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(ExposeTransform::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(Matrix3Controller::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(PRS::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(IKChainControl::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(IKControl::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(PositionList::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(RotationList::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(ScaleList::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

}
