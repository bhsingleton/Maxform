//
// File: RotationController.cpp
//
// Author: Benjamin H. Singleton
//

#include "RotationController.h"

MObject	RotationController::value;
MObject	RotationController::valueX;
MObject	RotationController::valueY;
MObject	RotationController::valueZ;

MString	RotationController::valueCategory("Value");

MString RotationController::classification("rotationController");
MTypeId	RotationController::id(0x0013b1d9);


RotationController::RotationController() { this->rotationController = MObjectHandle(); this->matrix3Controller = MObjectHandle(); };
RotationController::~RotationController() { this->rotationController = MObjectHandle(); this->matrix3Controller = MObjectHandle(); };


MStatus RotationController::compute(const MPlug& plug, MDataBlock& data)
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


MStatus RotationController::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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

	bool isValue = fnAttribute.hasCategory(RotationController::valueCategory);

	if (isValue && asSrc)
	{

		// Evaluate classification
		//
		MObject otherNode = otherPlug.node();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isRotationController = Maxformations::hasClassification(otherNode, RotationController::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isMatrix3Controller = Maxformations::hasClassification(otherNode, Matrix3Controller::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isRotationController && !this->rotationController.isAlive())
		{

			this->rotationController = MObjectHandle(otherNode);
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


MStatus RotationController::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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

	bool isValue = fnAttribute.hasCategory(RotationController::valueCategory);

	if (isValue && asSrc)
	{

		// Evaluate other node's classification
		//
		MObject otherNode = otherPlug.node();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isRotationController = Maxformations::hasClassification(otherNode, RotationController::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isMatrix3Controller = Maxformations::hasClassification(otherNode, Matrix3Controller::classification, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isPartiallyConnected = Maxformations::isPartiallyConnected(plug, false, true, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isRotationController && !isPartiallyConnected)
		{

			this->rotationController = MObjectHandle();

		}
		else if (isMatrix3Controller && !isPartiallyConnected)
		{

			this->matrix3Controller = MObjectHandle();

		}
		else;

	}

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


bool RotationController::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return true;

};


void* RotationController::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new RotationController();

};


void RotationController::dependentChanged(const MObject& otherNode)
/**
Notifies that a dependent of this node has changed.

@param node: The node that is dependent on this controller.
@return: Void.
*/
{

	return;

};


Maxform* RotationController::getAssociatedTransform(MStatus* status)
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
	else if (this->rotationController.isAlive())
	{

		MObject node = this->rotationController.object();

		MFnDependencyNode fnNode(node, status);
		CHECK_MSTATUS_AND_RETURN(*status, nullptr);

		MPxNode* userNode = fnNode.userNode(status);
		CHECK_MSTATUS_AND_RETURN(*status, nullptr);

		RotationController* controller = static_cast<RotationController*>(userNode);

		return controller->getAssociatedTransform(status);


	}
	else
	{

		return nullptr;

	}

};


MStatus RotationController::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{

	MStatus status;

	// Initialize function sets
	//
	MFnUnitAttribute fnUnitAttr;
	MFnNumericAttribute fnNumericAttr;

	// Define output attributes
	// ".valueX" attribute
	//
	RotationController::valueX = fnUnitAttr.create("valueX", "vx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationController::valueCategory));

	// ".valueY" attribute
	//
	RotationController::valueY = fnUnitAttr.create("valueY", "vy", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationController::valueCategory));

	// ".valueZ" attribute
	//
	RotationController::valueZ = fnUnitAttr.create("valueZ", "vz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationController::valueCategory));

	// ".value" attribute
	//
	RotationController::value = fnNumericAttr.create("value", "v", RotationController::valueX, RotationController::valueY, RotationController::valueZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationController::valueCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(RotationController::addAttribute(RotationController::value));

	return status;

};