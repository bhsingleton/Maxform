//
// File: Matrix3Controller.cpp
//
// Author: Benjamin H. Singleton
//

#include "Matrix3Controller.h"

MObject Matrix3Controller::IDENTITY_MATRIX_DATA;

MObject	Matrix3Controller::value;
MString	Matrix3Controller::valueCategory("Value");

MTypeId	Matrix3Controller::id(0x0013b1d1);


Matrix3Controller::Matrix3Controller() { this->maxform = nullptr; };
Matrix3Controller::~Matrix3Controller() { this->maxform = nullptr; };


MStatus Matrix3Controller::compute(const MPlug& plug, MDataBlock& data)
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


MStatus Matrix3Controller::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == Matrix3Controller::value && asSrc) && otherPlug == Maxform::transform)
	{

		// Store reference to maxform
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		this->maxform = static_cast<Maxform*>(fnDependNode.userNode(&status));
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	return MPxNode::connectionMade(plug, otherPlug, asSrc);

};


MStatus Matrix3Controller::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == Matrix3Controller::value && asSrc) && otherPlug == Maxform::transform)
	{

		// Cleanup reference to maxform
		//
		this->maxform = nullptr;

	}

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


bool Matrix3Controller::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return true;

};


void* Matrix3Controller::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new Matrix3Controller();

};


Maxform* Matrix3Controller::maxformPtr()
/**
Returns the maxform node associated with this matrix3 controller.
If no maxform node exists then a null pointer is returned instead!

@return: Maxform pointer.
*/
{

	return this->maxform;

};


MStatus Matrix3Controller::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{
	
	MStatus status;

	// Initialize function sets
	//
	MFnTypedAttribute fnTypedAttr;

	// Define default matrix value
	//
	Matrix3Controller::IDENTITY_MATRIX_DATA = Maxformations::createMatrixData(MTransformationMatrix::identity);

	// ".value" attribute
	//
	Matrix3Controller::value = fnTypedAttr.create("value", "v", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.setWritable(false));
	CHECK_MSTATUS(fnTypedAttr.setStorable(false));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(Matrix3Controller::valueCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(Matrix3Controller::addAttribute(Matrix3Controller::value));

	return status;

};