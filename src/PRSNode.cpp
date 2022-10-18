//
// File: PRSNode.cpp
//
// Dependency Graph Node: prs
//
// Author: Benjamin H. Singleton
//

#include "PRSNode.h"

MObject		PRS::position;
MObject		PRS::x_position;
MObject		PRS::y_position;
MObject		PRS::z_position;
MObject		PRS::axisOrder;
MObject		PRS::rotation;
MObject		PRS::x_rotation;
MObject		PRS::y_rotation;
MObject		PRS::z_rotation;
MObject		PRS::scale;
MObject		PRS::x_scale;
MObject		PRS::y_scale;
MObject		PRS::z_scale;

MObject		PRS::matrix;
MObject		PRS::inverseMatrix;

MString		PRS::inputCategory("Input");
MString		PRS::outputCategory("Output");
MString		PRS::positionCategory("Position");
MString		PRS::rotationCategory("Rotation");
MString		PRS::eulerRotationCategory("EulerRotation");
MString		PRS::scaleCategory("Scale");

MTypeId		PRS::id(0x0013b1cb);


PRS::PRS() { this->maxform = nullptr; };
PRS::~PRS() { this->maxform = nullptr; };


MStatus PRS::compute(const MPlug& plug, MDataBlock& data) 
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

	// Evaluate requested plug
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (fnAttribute.hasCategory(PRS::outputCategory))
	{
		
		// Get input data handles
		//
		MDataHandle positionHandle = data.inputValue(PRS::position, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		MDataHandle xPositionHandle = positionHandle.child(PRS::x_position);
		MDataHandle yPositionHandle = positionHandle.child(PRS::y_position);
		MDataHandle zPositionHandle = positionHandle.child(PRS::z_position);
		
		MDataHandle axisOrderHandle = data.inputValue(PRS::axisOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rotationHandle = data.inputValue(PRS::rotation, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		MDataHandle xRotationHandle = rotationHandle.child(PRS::x_rotation);
		MDataHandle yRotationHandle = rotationHandle.child(PRS::y_rotation);
		MDataHandle zRotationHandle = rotationHandle.child(PRS::z_rotation);

		MDataHandle scaleHandle = data.inputValue(PRS::scale, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		MDataHandle xScaleHandle = scaleHandle.child(PRS::x_scale);
		MDataHandle yScaleHandle = scaleHandle.child(PRS::y_scale);
		MDataHandle zScaleHandle = scaleHandle.child(PRS::z_scale);
		
		// Get position value
		//
		double xPosition = xPositionHandle.asDistance().asCentimeters();
		double yPosition = yPositionHandle.asDistance().asCentimeters();
		double zPosition = zPositionHandle.asDistance().asCentimeters();

		MVector position = MVector(xPosition, yPosition, zPosition);
		
		// Get rotation value
		//
		double xRotation = xRotationHandle.asAngle().asRadians();
		double yRotation = yRotationHandle.asAngle().asRadians();
		double zRotation = zRotationHandle.asAngle().asRadians();

		Maxformations::AxisOrder axisOrder = Maxformations::AxisOrder(axisOrderHandle.asShort());
		MVector eulerRotation = MVector(xRotation, yRotation, zRotation);

		// Get scale value
		//
		double xScale = xScaleHandle.asDouble();
		double yScale = yScaleHandle.asDouble();
		double zScale = zScaleHandle.asDouble();

		MVector scale = MVector(xScale, yScale, zScale);
		
		// Compose transform matrix
		//
		MMatrix positionMatrix = Maxformations::createPositionMatrix(position);
		MMatrix rotationMatrix = Maxformations::createRotationMatrix(eulerRotation, axisOrder);
		MMatrix scaleMatrix = Maxformations::createScaleMatrix(scale);

		MMatrix matrix = scaleMatrix * rotationMatrix * positionMatrix;
		
		// Get output data handles
		//
		MDataHandle matrixHandle = data.outputValue(PRS::matrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle inverseMatrixHandle = data.outputValue(PRS::inverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		matrixHandle.setMMatrix(matrix);
		matrixHandle.setClean();

		inverseMatrixHandle.setMMatrix(matrix.inverse());
		inverseMatrixHandle.setClean();

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


MStatus PRS::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == PRS::matrix && asSrc) && this->maxform == nullptr)
	{

		// Inspect other node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId otherId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (otherId == Maxform::id)
		{

			this->maxform = static_cast<Maxform*>(fnDependNode.userNode());

		}

	}

	return MPxNode::connectionMade(plug, otherPlug, asSrc);

};


MStatus PRS::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == PRS::matrix && asSrc) && this->maxform != nullptr)
	{

		this->maxform = nullptr;

	}

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


Maxform* PRS::maxformPtr()
/**
Returns the maxform node associated with this prs controller.
If no maxform node exists then a null pointer is returned instead!

@return: Maxform pointer.
*/
{

	return this->maxform;

};


void PRS::updateMaxformPtr(Maxform* maxform)
/**
Updates the internal maxform node pointer.
This function is intended for use by IK-Chain controllers!

@param maxform: The new maxform pointer.
@return: void
*/
{

	this->maxform = maxform;

};


void* PRS::creator() 
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: PRS
*/
{

	return new PRS();

};


MStatus PRS::initialize()
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
	MFnUnitAttribute fnUnitAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnEnumAttribute fnEnumAttr;
	MFnCompoundAttribute fnCompoundAttr;

	// Input attributes:
	// ".x_position" attribute
	//
	PRS::x_position = fnUnitAttr.create("x_position", "xp", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::positionCategory));

	// ".y_position" attribute
	//
	PRS::y_position = fnUnitAttr.create("y_position", "yp",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::positionCategory));
	
	// ".z_position" attribute
	//
	PRS::z_position = fnUnitAttr.create("z_position", "zp",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::positionCategory));
	
	// ".position" attribute
	//
	PRS::position = fnNumericAttr.create("position", "p", PRS::x_position, PRS::y_position, PRS::z_position, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::positionCategory));

	// ".axisOrder" attribute
	//
	PRS::axisOrder = fnEnumAttr.create("axisOrder", "ao", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 5));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 6));
	CHECK_MSTATUS(fnEnumAttr.addField("xyx", 7));
	CHECK_MSTATUS(fnEnumAttr.addField("yzy", 8));
	CHECK_MSTATUS(fnEnumAttr.addField("zxz", 9));
	CHECK_MSTATUS(fnEnumAttr.setChannelBox(true));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PRS::eulerRotationCategory));

	// ".x_rotation" attribute
	//
	PRS::x_rotation = fnUnitAttr.create("x_rotation", "xr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::rotationCategory));

	// ".y_rotation" attribute
	//
	PRS::y_rotation = fnUnitAttr.create("y_rotation", "yr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::rotationCategory));

	// ".z_rotation" attribute
	//
	PRS::z_rotation = fnUnitAttr.create("z_rotation", "zr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::rotationCategory));
	
	// ".rotation" attribute
	//
	PRS::rotation = fnNumericAttr.create("rotation", "r", PRS::x_rotation, PRS::y_rotation, PRS::z_rotation, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::rotationCategory));
	
	// ".x_scale" attribute
	//
	PRS::x_scale = fnNumericAttr.create("x_scale", "xs", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));

	// ".y_scale" attribute
	//
	PRS::y_scale = fnNumericAttr.create("y_scale", "ys",  MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));
	
	// ".z_scale" attribute
	//
	PRS::z_scale = fnNumericAttr.create("z_scale", "zs",  MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));
	
	// ".scale" attribute
	//
	PRS::scale = fnNumericAttr.create("scale", "s", PRS::x_scale, PRS::y_scale, PRS::z_scale, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));
	
	// Output attributes:
	// ".matrix" attribute
	//
	PRS::matrix = fnMatrixAttr.create("matrix", "m", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PRS::outputCategory));

	// ".inverseMatrix" attribute
	//
	PRS::inverseMatrix = fnMatrixAttr.create("inverseMatrix", "im", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PRS::outputCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(PRS::addAttribute(PRS::position));
	CHECK_MSTATUS(PRS::addAttribute(PRS::rotation));
	CHECK_MSTATUS(PRS::addAttribute(PRS::scale));
	CHECK_MSTATUS(PRS::addAttribute(PRS::axisOrder));

	CHECK_MSTATUS(PRS::addAttribute(PRS::matrix));
	CHECK_MSTATUS(PRS::addAttribute(PRS::inverseMatrix));

	// Define attribute relationships
	//
	CHECK_MSTATUS(PRS::attributeAffects(PRS::position, PRS::matrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::axisOrder, PRS::matrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::rotation, PRS::matrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::scale, PRS::matrix));

	CHECK_MSTATUS(PRS::attributeAffects(PRS::position, PRS::inverseMatrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::axisOrder, PRS::inverseMatrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::rotation, PRS::inverseMatrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::scale, PRS::inverseMatrix));

	return status;

};