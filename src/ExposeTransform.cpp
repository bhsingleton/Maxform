//
// File: ExposeTransform.cpp
//
// Dependency Graph Node: exposeTm
//
// Author: Benjamin H. Singleton
//

#include "ExposeTransform.h"

MObject		ExposeTransform::exposeNode;
MObject		ExposeTransform::exposeMatrix;
MObject		ExposeTransform::localReferenceNode;
MObject		ExposeTransform::localReferenceMatrix;
MObject		ExposeTransform::useParent;
MObject		ExposeTransform::eulerXOrder;
MObject		ExposeTransform::eulerYOrder;
MObject		ExposeTransform::eulerZOrder;
MObject		ExposeTransform::stripNUScale;
MObject		ExposeTransform::useTimeOffset;
MObject		ExposeTransform::timeOffset;

MObject		ExposeTransform::localPosition;
MObject		ExposeTransform::localPositionX;
MObject		ExposeTransform::localPositionY;
MObject		ExposeTransform::localPositionZ;
MObject		ExposeTransform::worldPosition;
MObject		ExposeTransform::worldPositionX;
MObject		ExposeTransform::worldPositionY;
MObject		ExposeTransform::worldPositionZ;
MObject		ExposeTransform::localEuler;
MObject		ExposeTransform::localEulerX;
MObject		ExposeTransform::localEulerY;
MObject		ExposeTransform::localEulerZ;
MObject		ExposeTransform::worldEuler;
MObject		ExposeTransform::worldEulerX;
MObject		ExposeTransform::worldEulerY;
MObject		ExposeTransform::worldEulerZ;
MObject		ExposeTransform::distance;
MObject		ExposeTransform::angle;

MString		ExposeTransform::exposeCategory("Expose");

MTypeId		ExposeTransform::id(0x0013b1c8);


ExposeTransform::ExposeTransform()
/**
Constructor.
*/
{

	this->exposeHandle = MObjectHandle();
	this->localReferenceHandle = MObjectHandle();
	this->parentEnabled = false;

};


ExposeTransform::~ExposeTransform() {};


MStatus ExposeTransform::compute(const MPlug& plug, MDataBlock& data) 
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

	if (fnAttribute.hasCategory(ExposeTransform::exposeCategory))
	{

		// Get current time
		//
		MDGContext currentContext = data.context(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTime currentTime;

		if (currentContext.isNormal())
		{

			currentTime = MAnimControl::currentTime();

		}
		else
		{

			status = currentContext.getTime(currentTime);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

		// Cache expose matrix
		//
		MDataHandle exposeMatrixHandle = data.inputValue(ExposeTransform::exposeMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localReferenceMatrixHandle = data.inputValue(ExposeTransform::localReferenceMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		unsigned int currentFrame = std::round(currentTime.value());
		MMatrix exposeMatrix = exposeMatrixHandle.asMatrix();
		MMatrix localReferenceMatrix = localReferenceMatrixHandle.asMatrix();

		this->exposeMatrices[currentFrame] = exposeMatrix;
		this->localReferenceMatrices[currentFrame] = localReferenceMatrix;

		// Evaluate time offset
		//
		MDataHandle useTimeOffsetHandle = data.inputValue(ExposeTransform::useTimeOffset, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle timeOffsetHandle = data.inputValue(ExposeTransform::timeOffset, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool useTimeOffset = useTimeOffsetHandle.asBool();
		MTime timeOffset = timeOffsetHandle.asTime();

		if (useTimeOffset)
		{

			// Get matrices at offset time
			//
			MTime offsetTime = currentTime + timeOffset;

			status = this->getCachedMatrices(offsetTime, exposeMatrix, localReferenceMatrix);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

		// Get input data handles
		//
		MDataHandle eulerXOrderHandle = data.inputValue(ExposeTransform::eulerXOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle eulerYOrderHandle = data.inputValue(ExposeTransform::eulerYOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle eulerZOrderHandle = data.inputValue(ExposeTransform::eulerZOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle stripNUScaleHandle = data.inputValue(ExposeTransform::stripNUScale, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		Maxformations::AxisOrder eulerXOrder = Maxformations::AxisOrder(eulerXOrderHandle.asShort());
		Maxformations::AxisOrder eulerYOrder = Maxformations::AxisOrder(eulerYOrderHandle.asShort());
		Maxformations::AxisOrder eulerZOrder = Maxformations::AxisOrder(eulerZOrderHandle.asShort());

		bool stripNUScale = stripNUScaleHandle.asBool();
		
		// Calculate local matrix
		//
		if (stripNUScale)
		{
			
			exposeMatrix = Maxformations::normalizeMatrix(exposeMatrix);
			localReferenceMatrix = Maxformations::normalizeMatrix(localReferenceMatrix);
			
		}
		
		MMatrix localMatrix = exposeMatrix * localReferenceMatrix.inverse();
		
		// Get transform values
		//
		MVector localPosition = Maxformations::matrixToPosition(localMatrix);
		double localEulerX = Maxformations::matrixToEulerAngles(localMatrix, eulerXOrder)[0];
		double localEulerY = Maxformations::matrixToEulerAngles(localMatrix, eulerYOrder)[1];
		double localEulerZ = Maxformations::matrixToEulerAngles(localMatrix, eulerZOrder)[2];

		MVector worldPosition = Maxformations::matrixToPosition(exposeMatrix);
		double worldEulerX = Maxformations::matrixToEulerAngles(exposeMatrix, eulerXOrder)[0];
		double worldEulerY = Maxformations::matrixToEulerAngles(exposeMatrix, eulerYOrder)[1];
		double worldEulerZ = Maxformations::matrixToEulerAngles(exposeMatrix, eulerZOrder)[2];

		MDistance distanceBetween = Maxformations::distanceBetween(localReferenceMatrix, exposeMatrix);
		MAngle angleBetween = Maxformations::angleBetween(localReferenceMatrix, exposeMatrix);

		// Get output data handles
		//
		MDataHandle localPositionXHandle = data.outputValue(ExposeTransform::localPositionX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localPositionYHandle = data.outputValue(ExposeTransform::localPositionY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localPositionZHandle = data.outputValue(ExposeTransform::localPositionZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localEulerXHandle = data.outputValue(ExposeTransform::localEulerX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localEulerYHandle = data.outputValue(ExposeTransform::localEulerY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localEulerZHandle = data.outputValue(ExposeTransform::localEulerZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldPositionXHandle = data.outputValue(ExposeTransform::worldPositionX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldPositionYHandle = data.outputValue(ExposeTransform::worldPositionY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldPositionZHandle = data.outputValue(ExposeTransform::worldPositionZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldEulerXHandle = data.outputValue(ExposeTransform::worldEulerX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldEulerYHandle = data.outputValue(ExposeTransform::worldEulerY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldEulerZHandle = data.outputValue(ExposeTransform::worldEulerZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle distanceHandle = data.outputValue(ExposeTransform::distance, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle angleHandle = data.outputValue(ExposeTransform::angle, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output handles
		//
		MDistance::Unit distanceUnit = MDistance::uiUnit();
		MAngle::Unit angleUnit = MAngle::internalUnit();

		localPositionXHandle.setMDistance(MDistance(localPosition.x, distanceUnit));
		localPositionXHandle.setClean();

		localPositionYHandle.setMDistance(MDistance(localPosition.y, distanceUnit));
		localPositionYHandle.setClean();

		localPositionZHandle.setMDistance(MDistance(localPosition.z, distanceUnit));
		localPositionZHandle.setClean();

		localEulerXHandle.setMAngle(MAngle(localEulerX, angleUnit));
		localEulerXHandle.setClean();

		localEulerYHandle.setMAngle(MAngle(localEulerY, angleUnit));
		localEulerYHandle.setClean();

		localEulerZHandle.setMAngle(MAngle(localEulerZ, angleUnit));
		localEulerZHandle.setClean();

		worldPositionXHandle.setMDistance(MDistance(worldPosition.x, distanceUnit));
		worldPositionXHandle.setClean();

		worldPositionYHandle.setMDistance(MDistance(worldPosition.y, distanceUnit));
		worldPositionYHandle.setClean();

		worldPositionZHandle.setMDistance(MDistance(worldPosition.z, distanceUnit));
		worldPositionZHandle.setClean();

		worldEulerXHandle.setMAngle(MAngle(worldEulerX, angleUnit));
		worldEulerXHandle.setClean();

		worldEulerYHandle.setMAngle(MAngle(worldEulerY, angleUnit));
		worldEulerYHandle.setClean();

		worldEulerZHandle.setMAngle(MAngle(worldEulerZ, angleUnit));
		worldEulerZHandle.setClean();

		distanceHandle.setMDistance(distanceBetween);
		distanceHandle.setClean();

		angleHandle.setMAngle(angleBetween);
		angleHandle.setClean();


		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else
	{

		return Maxform::compute(plug, data);

	}

};


bool ExposeTransform::setInternalValue(const MPlug& plug, const MDataHandle& handle)
/**
This method is overridden by nodes that store attribute data in some internal format.
The internal state of attributes can be set or queried using the setInternal and internal methods of MFnAttribute.
When internal attribute values are set via setAttr or MPlug::setValue this method is called.
Another use for this method is to impose attribute limits.

@param plug: The attribute that is being set.
@param handle: The dataHandle containing the value to set.
@return: Success.
*/
{

	MStatus status;

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN(status, false);

	if (attribute == ExposeTransform::useParent)
	{

		this->parentEnabled = handle.asBool();
		this->updateLocalReferenceMatrix();

	}

	return Maxform::setInternalValue(plug, handle);

};


MStatus ExposeTransform::getCachedMatrices(const MTime& time, MMatrix& exposeMatrix, MMatrix& localReferenceMatrix)
/**
Returns the expose and local reference matrix, at the specified time, from the internal cache.

@param time: Get the matrices at this time.
@param exposeMatrix: The passed expose matrix to populate.
@param localReferenceMatrix: The passed local reference matrix to populate.
@return: Status code.
*/
{

	MStatus status;

	// Check if time exists inside cache
	//
	unsigned int frame = std::round(time.value());
	bool hasMatrix = this->exposeMatrices.find(frame) != this->exposeMatrices.end();

	if (hasMatrix)
	{

		exposeMatrix = this->exposeMatrices[frame];
		localReferenceMatrix = this->localReferenceMatrices[frame];

	}

	return status;

};


MStatus ExposeTransform::legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal)
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
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if ((attribute == ExposeTransform::exposeNode || attribute == ExposeTransform::localReferenceNode) && !asSrc)
	{

		// Evaluate if other node is a transform
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = otherNode.hasFn(MFn::kTransform);

		return MS::kSuccess;

	}
	else
	{

		return Maxform::legalConnection(plug, otherPlug, asSrc, isLegal);

	}

};


MStatus ExposeTransform::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (attribute == ExposeTransform::exposeNode && !asSrc)
	{

		// Store node handle
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		this->exposeHandle = MObjectHandle(otherNode);
		this->updateExposeMatrix();
		this->updateLocalReferenceMatrix();

	}
	else if (attribute == ExposeTransform::localReferenceNode && !asSrc)
	{

		// Store node handle
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		this->localReferenceHandle = MObjectHandle(otherNode);
		this->updateLocalReferenceMatrix();

	}
	else;

	return Maxform::connectionMade(plug, otherPlug, asSrc);

};


MStatus ExposeTransform::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (attribute == ExposeTransform::exposeNode && !asSrc)
	{

		this->exposeHandle = MObjectHandle();

	}
	else if (attribute == ExposeTransform::localReferenceNode && !asSrc)
	{

		this->localReferenceHandle = MObjectHandle();

	}
	else;
	
	return Maxform::connectionBroken(plug, otherPlug, asSrc);

};


MStatus ExposeTransform::updateExposeMatrix()
/**
Ensures that the correct attributes are connected to the `exposeMatrix` attribute.
This function is only called when the `exposeNode` attribute is connected!

@return: Return status.
*/
{

	MStatus status;

	// Evaluate if file is being opened
	// Any DAG changes will interfere with file IO operations!
	//
	bool isSceneLoading = Maxformations::isSceneLoading();

	if (isSceneLoading)
	{

		return MS::kSuccess;

	}

	// Check if expose node is valid
	//
	bool isExposeNodeValid = this->exposeHandle.isValid() && this->exposeHandle.isAlive();

	if (isExposeNodeValid)
	{

		// Get dag path to expose node
		//
		MObject exposeNode = this->exposeHandle.object();

		MDagPath dagPath = MDagPath::getAPathTo(exposeNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		unsigned int instanceNumber = dagPath.instanceNumber(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get expose node's ".worldMatrix" plug
		//
		MFnDagNode fnDagNode(dagPath, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MPlug plug = fnDagNode.findPlug("worldMatrix", true, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MPlug element = plug.elementByLogicalIndex(instanceNumber, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get destination plug
		//
		MPlug otherPlug = MPlug(this->thisMObject(), ExposeTransform::exposeMatrix);

		// Connect plugs
		//
		status = Maxformations::connectPlugs(element, otherPlug, true);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}
	else
	{

		// Break connections to ".worldMatrix" plug
		//
		MPlug plug = MPlug(this->thisMObject(), ExposeTransform::exposeMatrix);

		status = Maxformations::breakConnections(plug, true, false);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	return status;

};


MStatus ExposeTransform::updateLocalReferenceMatrix()
/**
Ensures that the correct attributes are connected to the `localReferenceMatrix` attribute.
This function is only called when the `localReferenceNode` attribute is connected!

@return: Return status.
*/
{

	MStatus status;

	// Evaluate if file is being opened
	// Any DAG changes will interfere with file IO operations!
	//
	bool isSceneLoading = Maxformations::isSceneLoading();

	if (isSceneLoading)
	{

		return MS::kSuccess;

	}

	// Check if parent space is enabled
	//
	bool isExposeNodeValid = this->exposeHandle.isValid() && this->exposeHandle.isAlive();
	bool isLocalReferenceNodeValid = this->localReferenceHandle.isValid() && this->localReferenceHandle.isAlive();

	if (this->parentEnabled)
	{

		// Check if expose node is valid
		//
		if (isExposeNodeValid)
		{

			// Get dag path to expose node
			//
			MObject exposeNode = this->exposeHandle.object();

			MDagPath dagPath = MDagPath::getAPathTo(exposeNode, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			unsigned int instanceNumber = dagPath.instanceNumber(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get expose node's ".parentMatrix" plug
			//
			MFnDagNode fnDagNode(dagPath, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug plug = fnDagNode.findPlug("parentMatrix", true, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug element = plug.elementByLogicalIndex(instanceNumber, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get destination plug
			//
			MPlug otherPlug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			// Connect plugs
			//
			status = Maxformations::connectPlugs(element, otherPlug, true);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		else
		{

			// Break connections to ".localReferenceMatrix" plug
			//
			MPlug plug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			status = Maxformations::breakConnections(plug, true, false);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Reset plug value
			//
			status = Maxformations::resetMatrixPlug(plug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else
	{

		// Check if reference node is valid
		//
		if (isLocalReferenceNodeValid)
		{

			// Get dag path to local reference node
			//
			MObject localReferenceNode = this->localReferenceHandle.object();

			MDagPath dagPath = MDagPath::getAPathTo(localReferenceNode, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			unsigned int instanceNumber = dagPath.instanceNumber(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get local reference node's ".worldMatrix" plug
			//
			MFnDagNode fnDagNode(dagPath, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug plug = fnDagNode.findPlug("worldMatrix", true, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug element = plug.elementByLogicalIndex(instanceNumber, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get destination plug
			//
			MPlug otherPlug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			// Connect plugs
			//
			status = Maxformations::connectPlugs(element, otherPlug, true);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		else
		{

			// Break connections to ".localReferenceMatrix" plug
			//
			MPlug plug = MPlug(this->thisMObject(), ExposeTransform::localReferenceMatrix);

			status = Maxformations::breakConnections(plug, true, false);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Reset plug value
			//
			status = Maxformations::resetMatrixPlug(plug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}

	return status;

};


void* ExposeTransform::creator() 
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new ExposeTransform();

};


MStatus ExposeTransform::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{
	
	MStatus status;

	// Inherit attributes from parent class
	//
	status = ExposeTransform::inheritAttributesFrom("maxform");
	CHECK_MSTATUS_AND_RETURN_IT(status);

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
	// ".exposeNode" attribute
	//
	ExposeTransform::exposeNode = fnMessageAttr.create("exposeNode", "en", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".exposeMatrix" attribute
	//
	ExposeTransform::exposeMatrix = fnMatrixAttr.create("exposeMatrix", "em", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".localReferenceNode" attribute
	//
	ExposeTransform::localReferenceNode = fnMessageAttr.create("localReferenceNode", "lrn", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".localReferenceMatrix" attribute
	//
	ExposeTransform::localReferenceMatrix = fnMatrixAttr.create("localReferenceMatrix", "lrm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".useParent" attribute
	//
	ExposeTransform::useParent = fnNumericAttr.create("useParent", "up", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));

	// ".eulerXOrder" attribute
	//
	ExposeTransform::eulerXOrder = fnEnumAttr.create("eulerXOrder", "exo", short(1), &status);
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

	// ".eulerYOrder" attribute
	//
	ExposeTransform::eulerYOrder = fnEnumAttr.create("eulerYOrder", "eyo", short(1), &status);
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

	// ".eulerZOrder" attribute
	//
	ExposeTransform::eulerZOrder = fnEnumAttr.create("eulerZOrder", "ezo", short(1), &status);
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

	// ".stripNUScale" attribute
	//
	ExposeTransform::stripNUScale = fnNumericAttr.create("stripNUScale", "snus", MFnNumericData::kBoolean, false & status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	// ".useTimeOffset" attribute
	//
	ExposeTransform::useTimeOffset = fnNumericAttr.create("useTimeOffset", "uto", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".timeOffset" attribute
	//
	ExposeTransform::timeOffset = fnUnitAttr.create("timeOffset", "to", MFnUnitAttribute::kTime, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Output attributes:
	// ".localPositionX" attribute
	//
	ExposeTransform::localPositionX = fnUnitAttr.create("localPositionX", "lpx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".localPositionY" attribute
	//
	ExposeTransform::localPositionY = fnUnitAttr.create("localPositionY", "lpy", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".localPositionZ" attribute
	//
	ExposeTransform::localPositionZ = fnUnitAttr.create("localPositionZ", "lpz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".localPosition" attribute
	//
	ExposeTransform::localPosition = fnNumericAttr.create("localPosition", "lp", ExposeTransform::localPositionX, ExposeTransform::localPositionY, ExposeTransform::localPositionZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldPositionX" attribute
	//
	ExposeTransform::worldPositionX = fnUnitAttr.create("worldPositionX", "wpx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldPositionY" attribute
	//
	ExposeTransform::worldPositionY = fnUnitAttr.create("worldPositionY", "wpy", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldPositionZ" attribute
	//
	ExposeTransform::worldPositionZ = fnUnitAttr.create("worldPositionZ", "wpz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldPosition" attribute
	//
	ExposeTransform::worldPosition = fnNumericAttr.create("worldPosition", "wp", ExposeTransform::worldPositionX, ExposeTransform::worldPositionY, ExposeTransform::worldPositionZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".localEulerX" attribute
	//
	ExposeTransform::localEulerX = fnUnitAttr.create("localEulerX", "lex", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".localEulerY" attribute
	//
	ExposeTransform::localEulerY = fnUnitAttr.create("localEulerY", "ley", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".localEulerZ" attribute
	//
	ExposeTransform::localEulerZ = fnUnitAttr.create("localEulerZ", "lez", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".localEuler" attribute
	//
	ExposeTransform::localEuler = fnNumericAttr.create("localEuler", "le", ExposeTransform::localEulerX, ExposeTransform::localEulerY, ExposeTransform::localEulerZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldEulerX" attribute
	//
	ExposeTransform::worldEulerX = fnUnitAttr.create("worldEulerX", "wex", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldEulerY" attribute
	//
	ExposeTransform::worldEulerY = fnUnitAttr.create("worldEulerY", "wey", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldEulerZ" attribute
	//
	ExposeTransform::worldEulerZ = fnUnitAttr.create("worldEulerZ", "wez", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".worldEuler" attribute
	//
	ExposeTransform::worldEuler = fnNumericAttr.create("worldEuler", "we", ExposeTransform::worldEulerX, ExposeTransform::worldEulerY, ExposeTransform::worldEulerZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".distance" attribute
	//
	ExposeTransform::distance = fnUnitAttr.create("distance", "d", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// ".angle" attribute
	//
	ExposeTransform::angle = fnUnitAttr.create("angle", "a", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(ExposeTransform::exposeCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::exposeNode));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::exposeMatrix));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localReferenceNode));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localReferenceMatrix));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::useParent));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::eulerXOrder));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::eulerYOrder));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::eulerZOrder));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::stripNUScale));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::useTimeOffset));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::timeOffset));
	
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localPosition));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::worldPosition));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::localEuler));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::worldEuler));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::addAttribute(ExposeTransform::angle));

	// Define attribute relationships
	//
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localPosition));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::localEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::exposeMatrix, ExposeTransform::angle));

	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldPosition));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::worldEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::localReferenceMatrix, ExposeTransform::angle));

	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerXOrder, ExposeTransform::localEulerX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerYOrder, ExposeTransform::localEulerY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerZOrder, ExposeTransform::localEulerZ));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerXOrder, ExposeTransform::worldEulerX));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerYOrder, ExposeTransform::worldEulerY));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::eulerZOrder, ExposeTransform::worldEulerZ));

	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::stripNUScale, ExposeTransform::localEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::stripNUScale, ExposeTransform::worldEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::stripNUScale, ExposeTransform::angle));

	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::useTimeOffset, ExposeTransform::localPosition));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::useTimeOffset, ExposeTransform::worldPosition));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::useTimeOffset, ExposeTransform::localEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::useTimeOffset, ExposeTransform::worldEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::useTimeOffset, ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::useTimeOffset, ExposeTransform::angle));

	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::timeOffset, ExposeTransform::localPosition));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::timeOffset, ExposeTransform::worldPosition));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::timeOffset, ExposeTransform::localEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::timeOffset, ExposeTransform::worldEuler));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::timeOffset, ExposeTransform::distance));
	CHECK_MSTATUS(ExposeTransform::attributeAffects(ExposeTransform::timeOffset, ExposeTransform::angle));

	return status;

};