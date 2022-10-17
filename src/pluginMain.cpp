//
// File: pluginMain.cpp
//
// Author: Benjamin H. Singleton
//

#include "Maxform.h"
#include "PRSNode.h"
#include "PositionListNode.h"
#include "RotationListNode.h"
#include "ScaleListNode.h"
#include <maya/MFnPlugin.h>


MStatus initializePlugin(MObject obj) 
{

	MStatus status;

	// Register nodes
	//
	MFnPlugin plugin(obj, "Ben Singleton", "2017", "Any");

	status = plugin.registerTransform("maxform", Maxform::id, Maxform::creator, Maxform::initialize, Matrix3::creator, Matrix3::id, &Maxform::classification);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode("prs", PRS::id, PRS::creator, PRS::initialize);
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

	status = plugin.deregisterNode(PRS::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(PositionList::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(RotationList::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(ScaleList::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

}
