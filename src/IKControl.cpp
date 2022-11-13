//
// File: IKControl.cpp
//
// Dependency Graph Node: ikControl
//
// Author: Benjamin H. Singleton
//

#include "IKControl.h"

MObject	IKControl::ikSubControl;
MObject	IKControl::fkSubControl;
MObject	IKControl::preferredRotation;
MObject	IKControl::preferredRotationX;
MObject	IKControl::preferredRotationY;
MObject	IKControl::preferredRotationZ;

MString	IKControl::inputCategory("Input");
MTypeId	IKControl::id(0x0013b1d0);


IKControl::IKControl() : Matrix3Controller() { this->ikEnabled = false; this->prs = nullptr; };
IKControl::~IKControl() { this->prs = nullptr; };


MStatus IKControl::compute(const MPlug& plug, MDataBlock& data)
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

	MStatus status;

	// Check requested attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	bool isValue = fnAttribute.hasCategory(IKControl::valueCategory);

	if (isValue)
	{

		// Check if ik is enabled
		// 
		MDataHandle subControlHandle;

		if (this->ikEnabled)
		{

			subControlHandle = data.inputValue(IKControl::ikSubControl, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		else
		{

			subControlHandle = data.inputValue(IKControl::fkSubControl, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

		// Copy value to output data handle
		//
		MDataHandle valueHandle = data.outputValue(IKControl::value, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = valueHandle.setMObject(subControlHandle.data());
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else
	{

		return MS::kUnknownParameter;

	}

};


MStatus IKControl::legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal)
/**
This method allows you to check for legal connections being made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection if you are unable to determine if it is legal.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@param isLegal: Set this to true if the connection is legal, false otherwise.
@return: Return status.
*/
{

	MStatus status;

	// Inspect plug attribute
	//
	if (plug == IKControl::fkSubControl && asSrc)
	{

		// Evaluate if other node is a transform
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = fnDependNode.typeId() == PRS::id;

		return MS::kSuccess;

	}
	else if (plug == IKControl::ikSubControl && asSrc)
	{

		// Evaluate if other node is a transform
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = fnDependNode.typeId() == IKChainControl::id;

		return MS::kSuccess;

	}
	else
	{

		return MPxNode::legalConnection(plug, otherPlug, asSrc, isLegal);

	}

};


MStatus IKControl::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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

	// Inspect plug attribute
	//
	if ((plug == IKControl::fkSubControl && !asSrc) && otherPlug == PRS::value)
	{

		// Store reference to prs
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		this->prs = static_cast<PRS*>(fnDependNode.userNode(&status));
		CHECK_MSTATUS_AND_RETURN_IT(status);

		this->prs->registerMasterController(this);

	}
	else if ((plug == IKControl::ikSubControl && !asSrc) && otherPlug == IKChainControl::goal)
	{

		// Indicate ik is now enabled
		//
		this->ikEnabled = true;

	}
	else;

	return Matrix3Controller::connectionMade(plug, otherPlug, asSrc);

};


MStatus IKControl::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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

	// Inspect plug attribute
	//
	if ((plug == IKControl::fkSubControl && !asSrc) && otherPlug == PRS::value)
	{

		// Cleanup reference to prs
		//
		if (this->prs != nullptr)
		{

			this->prs->deregisterMasterController();
			this->prs = nullptr;

		}

	}
	else if (plug == IKControl::ikSubControl && !asSrc)
	{

		// Indicate ik is now disabled
		// All computations will now revert back to the fk sub-controller
		//
		this->ikEnabled = false;

	}
	else;

	return Matrix3Controller::connectionBroken(plug, otherPlug, asSrc);

};


bool IKControl::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return false;

};


void* IKControl::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new IKControl();

};


MStatus IKControl::initialize()
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
	MFnTypedAttribute fnTypedAttr;
	MFnUnitAttribute fnUnitAttr;
	MFnEnumAttribute fnEnumAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnCompoundAttribute fnCompoundAttr;
	MFnMessageAttribute fnMessageAttr;

	// Input attributes:
	// ".ikSubControl" attribute
	//
	IKControl::ikSubControl = fnTypedAttr.create("ikSubControl", "iksc", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".fkSubControl" attribute
	//
	IKControl::fkSubControl = fnTypedAttr.create("fkSubControl", "fksc", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	// ".preferredRotationX" attribute
	//
	IKControl::preferredRotationX = fnUnitAttr.create("preferredRotationX", "prx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".preferredRotationY" attribute
	//
	IKControl::preferredRotationY = fnUnitAttr.create("preferredRotationY", "pry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".preferredRotationZ" attribute
	//
	IKControl::preferredRotationZ = fnUnitAttr.create("preferredRotationZ", "prz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".preferredRotation" attribute
	//
	IKControl::preferredRotation = fnNumericAttr.create("preferredRotation", "pr", IKControl::preferredRotationX, IKControl::preferredRotationY, IKControl::preferredRotationZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));

	// Inherit attributes from parent class
	//
	status = PRS::inheritAttributesFrom("matrix3Controller");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Add attributes to node
	//
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::ikSubControl));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::fkSubControl));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::preferredRotation));

	// Define attribute relationships
	//
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::ikSubControl, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::fkSubControl, IKControl::value));

	return status;

};