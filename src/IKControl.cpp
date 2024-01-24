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
MObject	IKControl::rotationXActive;
MObject	IKControl::rotationXLimited;
MObject	IKControl::rotationXLowerLimit;
MObject	IKControl::rotationXUpperLimit;
MObject	IKControl::rotationYActive;
MObject	IKControl::rotationYLimited;
MObject	IKControl::rotationYLowerLimit;
MObject	IKControl::rotationYUpperLimit;
MObject	IKControl::rotationZActive;
MObject	IKControl::rotationZLimited;
MObject	IKControl::rotationZLowerLimit;
MObject	IKControl::rotationZUpperLimit;
MObject	IKControl::preferredRotation;
MObject	IKControl::preferredRotationX;
MObject	IKControl::preferredRotationY;
MObject	IKControl::preferredRotationZ;

MString	IKControl::inputCategory("Input");
MTypeId	IKControl::id(0x0013b1d0);


IKControl::IKControl() : Matrix3Controller() { this->ikEnabled = false; };
IKControl::~IKControl() {};


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

		// Check if IK is enabled
		// If not, then copy the FK sub-controller data handle
		// 
		if (!this->ikEnabled)
		{

			// Copy data handle
			//
			MDataHandle fkSubControlHandle = data.inputValue(IKControl::fkSubControl, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle valueHandle = data.outputValue(IKControl::value, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = valueHandle.copy(fkSubControlHandle);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Mark plug as clean
			//
			status = data.setClean(plug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			return MS::kSuccess;

		}

		// Evaluate limits and activations
		//
		MDataHandle rotationXActiveHandle = data.inputValue(IKControl::rotationXActive, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rotationYActiveHandle = data.inputValue(IKControl::rotationYActive, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rotationZActiveHandle = data.inputValue(IKControl::rotationZActive, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool rotationXActive = rotationXActiveHandle.asBool();
		bool rotationYActive = rotationYActiveHandle.asBool();
		bool rotationZActive = rotationZActiveHandle.asBool();
		bool rotationActive = rotationXActive && rotationYActive && rotationZActive;

		MDataHandle rotationXLimitedHandle = data.inputValue(IKControl::rotationXLimited, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rotationYLimitedHandle = data.inputValue(IKControl::rotationYLimited, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rotationZLimitedHandle = data.inputValue(IKControl::rotationZLimited, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool rotationXLimited = rotationXLimitedHandle.asBool();
		bool rotationYLimited = rotationYLimitedHandle.asBool();
		bool rotationZLimited = rotationZLimitedHandle.asBool();
		bool rotationLimited = rotationXLimited || rotationYLimited || rotationZLimited;

		if (rotationActive && !rotationLimited)
		{

			// Copy data handle
			//
			MDataHandle ikSubControlHandle = data.inputValue(IKControl::ikSubControl, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle valueHandle = data.outputValue(IKControl::value, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = valueHandle.copy(ikSubControlHandle);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Mark plug as clean
			//
			status = data.setClean(plug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			return MS::kSuccess;

		}

		// Get transformation matrices
		//
		MDataHandle ikSubControlHandle = data.inputValue(IKControl::ikSubControl, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle fkSubControlHandle = data.inputValue(IKControl::fkSubControl, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTransformationMatrix fkMatrix = Maxformations::getTransformData(fkSubControlHandle.data());
		MTransformationMatrix ikMatrix = Maxformations::getTransformData(ikSubControlHandle.data());

		MEulerRotation fkRotation = fkMatrix.eulerRotation();
		MEulerRotation ikRotation = ikMatrix.eulerRotation();

		// Apply x-rotation limit
		//
		if (!rotationXActive)
		{

			ikRotation.x = fkRotation.x;

		}
		else if (rotationXLimited)
		{

			MDataHandle rotationXLowerLimitHandle = data.inputValue(IKControl::rotationXLowerLimit, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			double rotationXLowerLimit = rotationXLowerLimitHandle.asAngle().asRadians();

			if (ikRotation.x < rotationXLowerLimit)
			{

				ikRotation.x = rotationXLowerLimit;

			}

			MDataHandle rotationXUpperLimitHandle = data.inputValue(IKControl::rotationXUpperLimit, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			double rotationXUpperLimit = rotationXUpperLimitHandle.asAngle().asRadians();

			if (ikRotation.x > rotationXUpperLimit)
			{

				ikRotation.x = rotationXUpperLimit;

			}

		}
		else;

		// Apply y-rotation limit
		//
		if (!rotationYActive)
		{

			ikRotation.y = fkRotation.y;

		}
		else if (rotationYLimited)
		{

			MDataHandle rotationYLowerLimitHandle = data.inputValue(IKControl::rotationYLowerLimit, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			double rotationYLowerLimit = rotationYLowerLimitHandle.asAngle().asRadians();

			if (ikRotation.y < rotationYLowerLimit)
			{

				ikRotation.y = rotationYLowerLimit;

			}

			MDataHandle rotationYUpperLimitHandle = data.inputValue(IKControl::rotationYUpperLimit, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			double rotationYUpperLimit = rotationYUpperLimitHandle.asAngle().asRadians();

			if (ikRotation.y > rotationYUpperLimit)
			{

				ikRotation.y = rotationYUpperLimit;

			}

		}
		else;

		// Apply z-rotation limit
		//
		if (!rotationZActive)
		{

			ikRotation.z = fkRotation.z;

		}
		else if (rotationZLimited)
		{

			MDataHandle rotationZLowerLimitHandle = data.inputValue(IKControl::rotationZLowerLimit, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			double rotationZLowerLimit = rotationZLowerLimitHandle.asAngle().asRadians();

			if (ikRotation.z < rotationZLowerLimit)
			{

				ikRotation.z = rotationZLowerLimit;

			}

			MDataHandle rotationZUpperLimitHandle = data.inputValue(IKControl::rotationZUpperLimit, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			double rotationZUpperLimit = rotationZUpperLimitHandle.asAngle().asRadians();

			if (ikRotation.z > rotationZUpperLimit)
			{

				ikRotation.z = rotationZUpperLimit;

			}

		}
		else;

		// Update transformation matrix
		//
		double3 eulerRotation = { ikRotation.x, ikRotation.y, ikRotation.z };
		MTransformationMatrix::RotationOrder rotationOrder = MTransformationMatrix::RotationOrder(fkRotation.order + 1);

		status = ikMatrix.setRotation(eulerRotation, rotationOrder);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MObject transformData = Maxformations::createMatrixData(ikMatrix);

		// Update data handle
		//
		MDataHandle valueHandle = data.outputValue(IKControl::value, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = valueHandle.setMObject(transformData);
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


void IKControl::getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const
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
	monitoredAttributes.append(IKControl::ikSubControl);
	monitoredAttributes.append(IKControl::fkSubControl);

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

		// Evaluate if other node is supported
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isPRS = Maxformations::hasTypeId(otherNode, PRS::id, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = isPRS;

		return MS::kSuccess;

	}
	else if (plug == IKControl::ikSubControl && asSrc)
	{

		// Evaluate if other node is supported
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isIKChainControl = Maxformations::hasTypeId(otherNode, IKChainControl::id, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isSplineIKChainControl = Maxformations::hasTypeId(otherNode, SplineIKChainControl::id, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = (isIKChainControl || isSplineIKChainControl);

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
	if (plug == IKControl::ikSubControl && !asSrc)
	{

		// Mark IK as enabled
		// All computations will be derived from the ik sub-controller!
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
	if (plug == IKControl::ikSubControl && !asSrc)
	{

		// Mark IK as disabled
		// All computations will now revert back to the fk sub-controller!
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
	
	// ".rotationXActive" attribute
	//
	IKControl::rotationXActive = fnNumericAttr.create("rotationXActive", "rxa", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));

	// ".rotationXLimited" attribute
	//
	IKControl::rotationXLimited = fnNumericAttr.create("rotationXLimited", "rxl", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));

	// ".rotationXLowerLimit" attribute
	//
	IKControl::rotationXLowerLimit = fnUnitAttr.create("rotationXLowerLimit", "rxll", MFnUnitAttribute::kAngle, -M_PI, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".rotationXUpperLimit" attribute
	//
	IKControl::rotationXUpperLimit = fnUnitAttr.create("rotationXUpperLimit", "rxul", MFnUnitAttribute::kAngle, M_PI, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".rotationYActive" attribute
	//
	IKControl::rotationYActive = fnNumericAttr.create("rotationYActive", "rya", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));

	// ".rotationYLimited" attribute
	//
	IKControl::rotationYLimited = fnNumericAttr.create("rotationYLimited", "ryl", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));

	// ".rotationYLowerLimit" attribute
	//
	IKControl::rotationYLowerLimit = fnUnitAttr.create("rotationYLowerLimit", "ryll", MFnUnitAttribute::kAngle, -M_PI, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".rotationYUpperLimit" attribute
	//
	IKControl::rotationYUpperLimit = fnUnitAttr.create("rotationYUpperLimit", "ryul", MFnUnitAttribute::kAngle, M_PI, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".rotationZActive" attribute
	//
	IKControl::rotationZActive = fnNumericAttr.create("rotationZActive", "rza", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));

	// ".rotationZLimited" attribute
	//
	IKControl::rotationZLimited = fnNumericAttr.create("rotationZLimited", "rzl", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));

	// ".rotationZLowerLimit" attribute
	//
	IKControl::rotationZLowerLimit = fnUnitAttr.create("rotationZLowerLimit", "rzll", MFnUnitAttribute::kAngle, -M_PI, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

	// ".rotationZUpperLimit" attribute
	//
	IKControl::rotationZUpperLimit = fnUnitAttr.create("rotationZUpperLimit", "rzul", MFnUnitAttribute::kAngle, M_PI, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));

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
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationXActive));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationXLimited));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationXLowerLimit));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationXUpperLimit));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationYActive));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationYLimited));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationYLowerLimit));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationYUpperLimit));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationZActive));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationZLimited));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationZLowerLimit));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::rotationZUpperLimit));
	CHECK_MSTATUS(IKControl::addAttribute(IKControl::preferredRotation));

	// Define attribute relationships
	//
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::ikSubControl, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::fkSubControl, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationXActive, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationXLimited, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationXLowerLimit, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationXUpperLimit, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationYActive, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationYLimited, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationYLowerLimit, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationYUpperLimit, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationZActive, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationZLimited, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationZLowerLimit, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::rotationZUpperLimit, IKControl::value));
	CHECK_MSTATUS(IKControl::attributeAffects(IKControl::preferredRotation, IKControl::value));

	return status;

};