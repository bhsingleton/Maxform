//
// File: PRS.cpp
//
// Dependency Graph Node: prs
//
// Author: Benjamin H. Singleton
//

#include "PRS.h"

MObject	PRS::position;
MObject	PRS::x_position;
MObject	PRS::y_position;
MObject	PRS::z_position;
MObject	PRS::axisOrder;
MObject	PRS::rotation;
MObject	PRS::x_rotation;
MObject	PRS::y_rotation;
MObject	PRS::z_rotation;
MObject	PRS::scale;
MObject	PRS::x_scale;
MObject	PRS::y_scale;
MObject	PRS::z_scale;

MString	PRS::inputCategory("Input");
MString	PRS::positionCategory("Position");
MString	PRS::rotationCategory("Rotation");
MString	PRS::scaleCategory("Scale");

MTypeId	PRS::id(0x0013b1cb);


PRS::PRS() { this->isSlaveController = false;  this->masterController = nullptr; };
PRS::~PRS() { this->masterController = nullptr; };


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
	if (plug == PRS::value)
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

		MTransformationMatrix::RotationOrder axisOrder = MTransformationMatrix::RotationOrder(axisOrderHandle.asShort() + 1);
		double3 eulerAngles = { xRotation, yRotation, zRotation };

		// Get scale value
		//
		double xScale = xScaleHandle.asDouble();
		double yScale = yScaleHandle.asDouble();
		double zScale = zScaleHandle.asDouble();

		double3 scale = { xScale, yScale, zScale };
		
		// Compose transform matrix
		//
		MTransformationMatrix transform = MTransformationMatrix(MMatrix::identity);
		transform.setTranslation(position, MSpace::kTransform);
		transform.setRotation(eulerAngles, axisOrder);
		transform.setScale(scale, MSpace::kTransform);

		MObject transformData = Maxformations::createMatrixData(transform);

		// Get output data handles
		//
		MDataHandle valueHandle = data.outputValue(PRS::value, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		valueHandle.setMObject(transformData);
		valueHandle.setClean();

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


void PRS::getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const
/**
Provide node-specific setup info for the Cached Playback system.

@param evaluationNode: This node's evaluation node, contains animated plug information.
@param disablingInfo: Information about why the node disables Cached Playback to be reported to the user.
@param cacheSetupInfo: Preferences and requirements this node has for Cached Playback.
@param monitoredAttributes: Attributes impacting the behavior of this method that will be monitored for change.
@return: void.
*/
{

	// Call parent function
	//
	MPxNode::getCacheSetup(evaluationNode, disablingInfo, cacheSetupInfo, monitoredAttributes);
	assert(!disablingInfo.getCacheDisabled());

	// Update caching preference
	//
	cacheSetupInfo.setPreference(MNodeCacheSetupInfo::kWantToCacheByDefault, true);

	// Append attributes for monitoring
	//
	monitoredAttributes.append(PRS::position);
	monitoredAttributes.append(PRS::x_position);
	monitoredAttributes.append(PRS::y_position);
	monitoredAttributes.append(PRS::z_position);
	monitoredAttributes.append(PRS::rotation);
	monitoredAttributes.append(PRS::x_rotation);
	monitoredAttributes.append(PRS::y_rotation);
	monitoredAttributes.append(PRS::z_rotation);
	monitoredAttributes.append(PRS::scale);
	monitoredAttributes.append(PRS::x_scale);
	monitoredAttributes.append(PRS::y_scale);
	monitoredAttributes.append(PRS::z_scale);

};


bool PRS::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return false;

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


void PRS::registerMasterController(Matrix3Controller* controller)
/**
Instructs this PRS node to call the supplied controller when retrieving the Maxform pointer.
This is useful for nested PRS nodes that are used under IK controllers.

@param controller: The master controller.
@return: Null.
*/
{

	this->masterController = controller;
	this->isSlaveController = true;

};


void PRS::deregisterMasterController()
/**
Removes all references to any registered master controllers.
Once called the PRS node will resume using the internal Maxform pointer.

@return: Null.
*/
{

	this->masterController = nullptr;
	this->isSlaveController = false;

};


Maxform* PRS::maxformPtr()
/**
Returns the maxform node associated with this matrix3 controller.
If no maxform node exists then a null pointer is returned instead!

@return: Maxform pointer.
*/
{

	if (this->isSlaveController && this->masterController != nullptr)
	{

		return this->masterController->maxformPtr();

	}
	else
	{

		return Matrix3Controller::maxformPtr();

	}

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
	PRS::axisOrder = fnEnumAttr.create("axisOrder", "ao", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 5));
	CHECK_MSTATUS(fnEnumAttr.setChannelBox(true));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PRS::rotationCategory));

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
	
	// Inherit attributes from parent class
	//
	status = PRS::inheritAttributesFrom("matrix3Controller");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Add attributes to node
	//
	CHECK_MSTATUS(PRS::addAttribute(PRS::position));
	CHECK_MSTATUS(PRS::addAttribute(PRS::rotation));
	CHECK_MSTATUS(PRS::addAttribute(PRS::scale));
	CHECK_MSTATUS(PRS::addAttribute(PRS::axisOrder));

	// Define attribute relationships
	//
	CHECK_MSTATUS(PRS::attributeAffects(PRS::position, PRS::value));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::axisOrder, PRS::value));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::rotation, PRS::value));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::scale, PRS::value));

	return status;

};