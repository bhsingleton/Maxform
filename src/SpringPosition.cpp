//
// File: SpringPosition.cpp
//
// Dependency Graph Node: springPosition
//
// Author: Benjamin H. Singleton
//

#include "SpringPosition.h"

MObject	SpringPosition::absolute;
MObject	SpringPosition::mass;
MObject	SpringPosition::drag;
MObject	SpringPosition::tension;
MObject	SpringPosition::dampening;
MObject	SpringPosition::position;
MObject	SpringPosition::x_position;
MObject	SpringPosition::y_position;
MObject	SpringPosition::z_position;
MObject	SpringPosition::startTime;
MObject	SpringPosition::time;
MObject	SpringPosition::steps;
MObject	SpringPosition::effect;
MObject	SpringPosition::x_effect;
MObject	SpringPosition::y_effect;
MObject	SpringPosition::z_effect;
MObject	SpringPosition::parentInverseMatrix;

MObject	SpringPosition::matrix;
MObject	SpringPosition::inverseMatrix;

MString	SpringPosition::springCategory("Spring");
MString	SpringPosition::positionCategory("Position");
MString	SpringPosition::effectCategory("Effect");

MTypeId	SpringPosition::id(0x0013b1d4);


SpringPosition::SpringPosition() : PositionController() {};
SpringPosition::~SpringPosition() {};


MStatus SpringPosition::compute(const MPlug& plug, MDataBlock& data)
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

	bool isValue = fnAttribute.hasCategory(SpringPosition::valueCategory);

	if (isValue)
	{

		// Copy input data handle
		//
		MDataHandle positionHandle = data.inputValue(SpringPosition::position, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueHandle = data.outputValue(SpringPosition::value, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		valueHandle.copy(positionHandle);

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


bool SpringPosition::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return false;

};


void* SpringPosition::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: SpringPosition
*/
{

	return new SpringPosition();

};


MStatus SpringPosition::initialize()
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
	MFnMatrixAttribute fnMatrixAttr;

	// Input attributes:
	// ".absolute" attribute
	//
	SpringPosition::absolute = fnNumericAttr.create("absolute", "abs", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".mass" attribute
	//
	SpringPosition::mass = fnNumericAttr.create("mass", "ms", MFnNumericData::kDouble, 300.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(1e-3));  // Avoids divide by zero errors
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".velocity" attribute
	//
	SpringPosition::drag = fnNumericAttr.create("drag", "drg", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".tension" attribute
	//
	SpringPosition::tension = fnNumericAttr.create("tension", "tns", MFnNumericData::kDouble, 2.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".dampening" attribute
	//
	SpringPosition::dampening = fnNumericAttr.create("dampening", "dmp", MFnNumericData::kDouble, 0.5, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".x_position" attribute
	//
	SpringPosition::x_position = fnUnitAttr.create("x_position", "xp", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(SpringPosition::positionCategory));

	// ".y_position" attribute
	//
	SpringPosition::y_position = fnUnitAttr.create("y_position", "yp", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(SpringPosition::positionCategory));

	// ".z_position" attribute
	//
	SpringPosition::z_position = fnUnitAttr.create("z_position", "zp", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(SpringPosition::positionCategory));

	// ".position" attribute
	//
	SpringPosition::position = fnNumericAttr.create("position", "p", SpringPosition::x_position, SpringPosition::y_position, SpringPosition::z_position, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::positionCategory));

	// ".startTime" attribute
	//
	SpringPosition::startTime = fnUnitAttr.create("startTime", "st", MFnUnitAttribute::kTime, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".time" attribute
	//
	SpringPosition::time = fnUnitAttr.create("time", "t", MFnUnitAttribute::kTime, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".iterations" attribute
	//
	SpringPosition::steps = fnNumericAttr.create("steps", "s", MFnNumericData::kInt, 2, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0));
	CHECK_MSTATUS(fnNumericAttr.setMax(4));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// ".x_effect" attribute
	//
	SpringPosition::x_effect = fnNumericAttr.create("x_effect", "xe", MFnNumericData::kDouble, 100.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(200.0));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::effectCategory));

	// ".y_effect" attribute
	//
	SpringPosition::y_effect = fnNumericAttr.create("y_effect", "ye", MFnNumericData::kDouble, 100.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(200.0));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::effectCategory));

	// ".z_effect" attribute
	//
	SpringPosition::z_effect = fnNumericAttr.create("z_effect", "ze", MFnNumericData::kDouble, 100.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(200.0));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::effectCategory));

	// ".effect" attribute
	//
	SpringPosition::effect = fnNumericAttr.create("effect", "e", SpringPosition::x_effect, SpringPosition::y_effect, SpringPosition::z_effect, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::effectCategory));

	// ".parentInverseMatrix" attribute
	//
	SpringPosition::parentInverseMatrix = fnMatrixAttr.create("parentInverseMatrix", "pim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(SpringPosition::springCategory));

	// Output attributes:
	// ".matrix" attribute
	//
	SpringPosition::matrix = fnMatrixAttr.create("matrix", "m", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(SpringPosition::valueCategory));

	// ".inverseMatrix" attribute
	//
	SpringPosition::inverseMatrix = fnMatrixAttr.create("inverseMatrix", "im", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(SpringPosition::valueCategory));

	// Inherit attributes from parent class
	//
	status = SpringPosition::inheritAttributesFrom("positionController");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Add attributes to node
	//
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::mass));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::drag));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::tension));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::dampening));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::position));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::startTime));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::time));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::steps));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::effect));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::parentInverseMatrix));

	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::matrix));
	CHECK_MSTATUS(SpringPosition::addAttribute(SpringPosition::inverseMatrix));

	// Define attribute relationships
	//
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::mass, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::drag, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::tension, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::dampening, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::position, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::startTime, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::time, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::steps, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::effect, SpringPosition::value));
	CHECK_MSTATUS(SpringPosition::attributeAffects(SpringPosition::parentInverseMatrix, SpringPosition::value));

	return status;

};