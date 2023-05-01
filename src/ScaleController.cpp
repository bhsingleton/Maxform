//
// File: ScaleController.cpp
//
// Author: Benjamin H. Singleton
//

#include "ScaleController.h"

MObject	ScaleController::value;
MObject	ScaleController::valueX;
MObject	ScaleController::valueY;
MObject	ScaleController::valueZ;

MString	ScaleController::valueCategory("Value");

MString ScaleController::classification("scaleController");
MTypeId	ScaleController::id(0x0013b1da);


ScaleController::ScaleController() { this->scaleController = MObjectHandle(); this->matrix3Controller = MObjectHandle(); };
ScaleController::~ScaleController() { this->scaleController = MObjectHandle(); this->matrix3Controller = MObjectHandle(); };


MStatus ScaleController::compute(const MPlug& plug, MDataBlock& data)
/**
This method should be overridden in user defined nodes.
Recompute the given output based on the nodes inputs.
The plug represents the data value that needs to be recomputed, and the data block holds the storage for all of the node's attributes.
The MDataBlock will provide smart handles for reading and writing this node's attribute values.
Only these values should be used when performing computations!

@param plug: Plug representing the attribute that needs to be recomputed.
@param data: Data block containing storage for the node's attributes.
@return: Return status.
*/
{

	return MS::kUnknownParameter;

};


MStatus ScaleController::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
/**
This method gets called when connections are made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection or if you want maya to process the connection as well.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@return: Return status.
*/
{

	MStatus status;

	// Inspect plug
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	bool isValue = fnAttribute.hasCategory(ScaleController::valueCategory);

	if (isValue && asSrc)
	{

		// Evaluate classification
		//
		MObject otherNode = otherPlug.node();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isScaleController = Maxformations::hasClassification(otherNode, ScaleController::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isMatrix3Controller = Maxformations::hasClassification(otherNode, Matrix3Controller::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isScaleController && !this->scaleController.isAlive())
		{

			this->scaleController = MObjectHandle(otherNode);
			this->dependentChanged(otherNode);

		}
		else if (isMatrix3Controller && !this->matrix3Controller.isAlive())
		{

			this->matrix3Controller = MObjectHandle(otherNode);
			this->dependentChanged(otherNode);

		}
		else;

	}

	return MPxNode::connectionMade(plug, otherPlug, asSrc);

};


MStatus ScaleController::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
/**
This method gets called when connections are made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection or if you want maya to process the connection as well.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@return: Return status.
*/
{

	MStatus status;

	// Inspect plug
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	bool isValue = fnAttribute.hasCategory(ScaleController::valueCategory);

	if (isValue && asSrc)
	{

		// Evaluate other node's classification
		//
		MObject otherNode = otherPlug.node();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isScaleController = Maxformations::hasClassification(otherNode, ScaleController::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isMatrix3Controller = Maxformations::hasClassification(otherNode, Matrix3Controller::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isPartiallyConnected = Maxformations::isPartiallyConnected(plug, false, true, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isScaleController && !isPartiallyConnected)
		{

			this->scaleController = MObjectHandle();

		}
		else if (isMatrix3Controller && !isPartiallyConnected)
		{

			this->matrix3Controller = MObjectHandle();

		}
		else;

	}

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


bool ScaleController::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return true;

};


void* ScaleController::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new ScaleController();

};


void ScaleController::dependentChanged(const MObject& otherNode)
/**
Notifies that a dependent of this node has changed.

@param node: The node that is dependent on this controller.
@return: Void.
*/
{

	return;

};


Maxform* ScaleController::getAssociatedTransform(MStatus* status)
/**
Returns the maxform node associated with this rotation controller.
If no maxform node exists then a null pointer is returned instead!

@param status: Return status.
@return: Maxform pointer.
*/
{
	
	if (this->matrix3Controller.isAlive())
	{

		MObject node = this->matrix3Controller.object();

		MFnDependencyNode fnNode(node, status);
		CHECK_MSTATUS_AND_RETURN(*status, nullptr);

		MPxNode* userNode = fnNode.userNode(status);
		CHECK_MSTATUS_AND_RETURN(*status, nullptr);

		Matrix3Controller* controller = static_cast<Matrix3Controller*>(userNode);

		return controller->getAssociatedTransform(status);

	}
	else if (this->scaleController.isAlive())
	{

		MObject node = this->scaleController.object();

		MFnDependencyNode fnNode(node, status);
		CHECK_MSTATUS_AND_RETURN(*status, nullptr);

		MPxNode* userNode = fnNode.userNode(status);
		CHECK_MSTATUS_AND_RETURN(*status, nullptr);

		ScaleController* controller = static_cast<ScaleController*>(userNode);

		return controller->getAssociatedTransform(status);

	}
	else
	{

		return nullptr;

	}

};


MStatus ScaleController::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{

	MStatus status;

	// Initialize function sets
	//
	MFnNumericAttribute fnNumericAttr;

	// Define output attributes
	// ".valueX" attribute
	//
	ScaleController::valueX = fnNumericAttr.create("valueX", "vx", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleController::valueCategory));

	// ".valueY" attribute
	//
	ScaleController::valueY = fnNumericAttr.create("valueY", "vy", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleController::valueCategory));

	// ".valueZ" attribute
	//
	ScaleController::valueZ = fnNumericAttr.create("valueZ", "vz", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleController::valueCategory));

	// ".value" attribute
	//
	ScaleController::value = fnNumericAttr.create("value", "v", ScaleController::valueX, ScaleController::valueY, ScaleController::valueZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleController::valueCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(ScaleController::addAttribute(ScaleController::value));

	return status;

};