//
// File: IKChainControl.cpp
//
// Dependency Graph Node: ikChainControl
//
// Author: Benjamin H. Singleton
//

#include "IKChainControl.h"

MObject	IKChainControl::enabled;
MObject	IKChainControl::ikGoal;
MObject IKChainControl::ikParentMatrix;
MObject	IKChainControl::forwardAxis;
MObject	IKChainControl::forwardAxisFlip;
MObject	IKChainControl::upAxis;
MObject	IKChainControl::upAxisFlip;
MObject	IKChainControl::joint;
MObject	IKChainControl::jointPreferredRotation;
MObject	IKChainControl::jointPreferredRotationX;
MObject	IKChainControl::jointPreferredRotationY;
MObject	IKChainControl::jointPreferredRotationZ;
MObject	IKChainControl::jointOffsetRotation;
MObject	IKChainControl::jointOffsetRotationX;
MObject	IKChainControl::jointOffsetRotationY;
MObject	IKChainControl::jointOffsetRotationZ;
MObject	IKChainControl::jointMatrix;
MObject	IKChainControl::jointParentMatrix;
MObject	IKChainControl::swivelAngle;
MObject	IKChainControl::useVHTarget;
MObject	IKChainControl::vhTarget;

MObject	IKChainControl::goal;

MString	IKChainControl::inputCategory("Input");
MString	IKChainControl::goalCategory("Goal");

MTypeId	IKChainControl::id(0x0013b1cf);


IKChainControl::IKChainControl() : Matrix3Controller() { this->prs = nullptr; };
IKChainControl::~IKChainControl() { this->prs = nullptr; };


MStatus IKChainControl::compute(const MPlug& plug, MDataBlock& data)
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

	bool isValue = fnAttribute.hasCategory(IKChainControl::valueCategory);
	bool isGoal = fnAttribute.hasCategory(IKChainControl::goalCategory);
	
	if (isValue)
	{

		// Copy value from data handle
		//
		MDataHandle ikGoalHandle = data.inputValue(IKChainControl::ikGoal, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueHandle = data.outputValue(IKChainControl::value, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		valueHandle.copy(ikGoalHandle);

		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else if (isGoal)
	{
		
		// Check if IK is enabled
		//
		MDataHandle enabledHandle = data.inputValue(IKChainControl::enabled, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle jointParentMatrixHandle = data.inputValue(IKChainControl::jointParentMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle jointArrayHandle = data.inputArrayValue(IKChainControl::joint, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool enabled = enabledHandle.asBool();

		MMatrix jointParentMatrix = Maxformations::getMatrixData(jointParentMatrixHandle.data());
		std::vector<IKControlSpec> joints = IKChainControl::getJoints(jointArrayHandle, jointParentMatrix);

		unsigned int numJoints = static_cast<unsigned int>(joints.size());

		if (enabled && numJoints > 0)
		{

			// Get input handles
			//
			MDataHandle ikGoalHandle = data.inputValue(IKChainControl::ikGoal, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle ikParentMatrixHandle = data.inputValue(IKChainControl::ikParentMatrix, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle forwardAxisHandle = data.inputValue(IKChainControl::forwardAxis, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle forwardAxisFlipHandle = data.inputValue(IKChainControl::forwardAxisFlip, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle upAxisHandle = data.inputValue(IKChainControl::upAxis, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle upAxisFlipHandle = data.inputValue(IKChainControl::upAxisFlip, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle swivelAngleHandle = data.inputValue(IKChainControl::swivelAngle, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle useVHTargetHandle = data.inputValue(IKChainControl::useVHTarget, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle vhTargetHandle = data.inputValue(IKChainControl::vhTarget, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get ik-goal matrix
			//
			MMatrix ikParentMatrix = Maxformations::getMatrixData(ikParentMatrixHandle.data());
			MMatrix ikGoal = Maxformations::getMatrixData(ikGoalHandle.data()) * ikParentMatrix;

			// Get ik system settings
			//
			int forwardAxis = forwardAxisHandle.asShort();
			bool forwardAxisFlip = forwardAxisFlipHandle.asBool();
			int upAxis = upAxisHandle.asShort();
			bool upAxisFlip = upAxisFlipHandle.asBool();
			MAngle swivelAngle = swivelAngleHandle.asAngle();

			// Get vector-handle target
			//
			bool useVHTarget = useVHTargetHandle.asBool();
			MMatrix vhTarget = Maxformations::getMatrixData(vhTargetHandle.data());

			MVector upVector;

			if (useVHTarget)
			{

				upVector = IKChainControl::getUpVector(joints[0].matrix, vhTarget);

			}
			else
			{

				upVector = IKChainControl::guessUpVector(joints, upAxis, upAxisFlip);

			}

			// Solve ik system
			//
			MMatrixArray worldMatrices = IKChainControl::solve(ikGoal, upVector, swivelAngle, joints);

			if ((forwardAxis != 0 || !forwardAxisFlip) || upAxis != 1 || !upAxisFlip)
			{

				status = Maxformations::reorientMatrices(worldMatrices, forwardAxis, forwardAxisFlip, upAxis, upAxisFlip);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

			// Localize solution back into parent space
			//
			MMatrixArray offsetMatrices = MMatrixArray(numJoints);
			MMatrixArray matrices = MMatrixArray(numJoints);
			MMatrix parentMatrix;

			for (unsigned int i = 0; i < numJoints; i++)
			{

				offsetMatrices[i] = joints[i].offsetRotation.asMatrix() * worldMatrices[i];
				parentMatrix = (i == 0) ? jointParentMatrix : offsetMatrices[i - 1];

				matrices[i] = offsetMatrices[i] * parentMatrix.inverse();

			}

			// Update output handles
			//
			MArrayDataHandle goalArrayHandle = data.outputArrayValue(IKChainControl::goal, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MArrayDataBuilder builder = goalArrayHandle.builder(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle goalHandle;
			unsigned int numMatrices = matrices.length();

			for (unsigned int i = 0; i < numMatrices; i++)
			{

				goalHandle = builder.addElement(i, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				status = goalHandle.setMObject(Maxformations::createMatrixData(matrices[i]));
				CHECK_MSTATUS_AND_RETURN_IT(status);

				goalHandle.setClean();

			}

			status = goalArrayHandle.set(builder);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = goalArrayHandle.setAllClean();
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		else
		{

			// Copy matrices back to array handle
			//
			MArrayDataHandle goalArrayHandle = data.outputArrayValue(IKChainControl::goal, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MArrayDataBuilder builder = goalArrayHandle.builder(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle goalHandle;

			for (unsigned int i = 0; i < numJoints; i++)
			{

				goalHandle = builder.addElement(i, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				status = goalHandle.setMObject(Maxformations::createMatrixData(joints[i].matrix));
				CHECK_MSTATUS_AND_RETURN_IT(status);

				goalHandle.setClean();

			}

			status = goalArrayHandle.set(builder);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = goalArrayHandle.setAllClean();
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

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


std::vector<IKControlSpec> IKChainControl::getJoints(MArrayDataHandle& arrayHandle, const MMatrix& parentMatrix)
/**
Returns an array of ik-control specs from the supplied array handle.

@param arrayHandle: The array data handle to extract from.
@return: The joint values.
*/
{

	MStatus status;

	// Presize joint array
	//
	unsigned int numElements = arrayHandle.elementCount();
	std::vector<IKControlSpec> joints = std::vector<IKControlSpec>(numElements);

	// Iterate through elements
	//
	MDataHandle elementHandle, preferredRotationHandle, offsetRotationHandle, matrixHandle;
	MEulerRotation preferredRotation, offsetRotation;
	MMatrix matrix;
	double length = 0.0;

	for (unsigned int i = 0; i < numElements; i++)
	{

		// Go to array element handle
		//
		status = arrayHandle.jumpToArrayElement(i);
		CHECK_MSTATUS_AND_RETURN(status, joints);

		elementHandle = arrayHandle.inputValue(&status);
		CHECK_MSTATUS_AND_RETURN(status, joints);

		// Get preferred rotation
		//
		preferredRotationHandle = elementHandle.child(IKChainControl::jointPreferredRotation);
		preferredRotation = MEulerRotation(preferredRotationHandle.asDouble3());

		// Get offset rotation
		//
		offsetRotationHandle = elementHandle.child(IKChainControl::jointOffsetRotation);
		offsetRotation = MEulerRotation(offsetRotationHandle.asDouble3());

		// Get joint's transform matrix
		//
		matrixHandle = elementHandle.child(IKChainControl::jointMatrix);
		matrix = Maxformations::getMatrixData(matrixHandle.data());
		length = i > 0 ? Maxformations::matrixToPosition(matrix).length() : 0.0;

		if (i == 0)
		{

			matrix *= parentMatrix;

		}

		joints[i] = IKControlSpec{ preferredRotation, offsetRotation, matrix, length };

	}

	return joints;

};


MMatrixArray IKChainControl::getMatrices(const std::vector<IKControlSpec>& joints)
{

	size_t numJoints = joints.size();
	MMatrixArray matrices = MMatrixArray(numJoints);

	for (size_t i = 0; i < numJoints; i++)
	{

		matrices[i] = joints[i].matrix;

	}

	return matrices;

};


MVector IKChainControl::getUpVector(const MMatrix& startJoint, const MMatrix& vhTarget)
/**
Returns the up-vector using the supplied ik-goal and vh-target.
If the points are overlapping then the scene's up-vector is used instead.

@param ikGoal: The ik-goal matrix.
@param vhTarget: The vh-target matrix.
@return: The up-vector.
*/
{

	MVector startPoint = Maxformations::matrixToPosition(startJoint);
	MVector vhPoint = Maxformations::matrixToPosition(vhTarget);

	MVector upVector = (vhPoint - startPoint).normal();

	if (upVector.length() > 0.0)
	{

		return upVector;

	}
	else
	{

		return Maxformations::getSceneUpVector();

	}

};


MVector IKChainControl::guessUpVector(const std::vector<IKControlSpec>& joints, const int upAxis, const bool upAxisFlip)
/**
Returns the best up-vector that could be used from the supplied joint matrices.

@param joint: The FK joint chain.
@param upAxis: The up-axis for the joint chain.
@param upAxisFlip: Determines if the up-axis is flipped.
@return: The best VH target matrix.
*/
{

	size_t numJoints = joints.size();

	switch (numJoints)
	{

		case 0:
		{

			// Default to scene up-vector
			//
			return Maxformations::getSceneUpVector();

		}
		break;

		case 1: case 2:
		{

			// Use up-axis vector
			//
			return upAxisFlip ? -MVector(joints[0].matrix[upAxis]) : MVector(joints[0].matrix[upAxis]);

		}
		break;

		default:
		{

			// Get start and end points
			//
			MMatrixArray matrices = IKChainControl::getMatrices(joints);
			MMatrixArray worldMatrices = Maxformations::expandMatrices(matrices);

			MMatrix startMatrix = worldMatrices[0];
			MVector startPoint = Maxformations::matrixToPosition(worldMatrices[0]);

			MMatrix endMatrix = worldMatrices[numJoints - 1];
			MVector endPoint = Maxformations::matrixToPosition(endMatrix);
			
			// Compose aim-vector
			//
			MVector forwardVector = (endPoint - startPoint).normal();
			MVector upVector = upAxisFlip ? -MVector(startMatrix[upAxis]) : MVector(startMatrix[upAxis]);
			MVector rightVector = (forwardVector ^ upVector).normal();

			return (rightVector ^ forwardVector).normal();

		}
		break;

	}

};


MMatrixArray IKChainControl::solve(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const std::vector<IKControlSpec>& joints)
/**
Returns a solution for the supplied joint matrices.
The solution uses the default forward-x and up-y axixes!

@param ikGoal: The IK goal in world space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param joints: The joints in their respective parent spaces.
@return: The joint matrices in their respective parent spaces.
*/
{

	unsigned int numItems = static_cast<unsigned int>(joints.size());

	switch (numItems)
	{

	case 0:
		return MMatrixArray();

	case 1:
		return IKChainControl::solve1Bone(ikGoal, upVector, swivelAngle, joints[0], 0.0);

	case 2:
		return IKChainControl::solve1Bone(ikGoal, upVector, swivelAngle, joints[0], joints[1]);

	case 3:
		return IKChainControl::solve2Bone(ikGoal, upVector, swivelAngle, joints[0], joints[1], joints[2]);

	default:
		return IKChainControl::solveNBone(ikGoal, upVector, swivelAngle, joints);

	}

};


MMatrixArray IKChainControl::solve1Bone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const IKControlSpec& startJoint, const double length)
/**
Solves a 1-bone system using aim-vector math.
All matrices are in the transform space of the previous joint.
The solution uses the default forward-x and up-y axixes, be sure to use `Maxformations::reorientMatrices` to change this!

@param ikGoal: The IK goal in world space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param startJoint: The start joint in world space.
@param endJoint: The end joint in parent space.
@return: The joint matrices in their respective parent spaces.
*/
{

	// Calculate forward vector
	//
	MVector startPoint = Maxformations::matrixToPosition(startJoint.matrix);
	MVector goalPoint = Maxformations::matrixToPosition(ikGoal);

	MVector aimVector = goalPoint - startPoint;
	MVector forwardVector = aimVector.normal();

	// Calculate axis vectors
	//
	MVector xAxis = forwardVector;
	MVector zAxis = (xAxis ^ upVector).normal();
	MVector yAxis = (zAxis ^ xAxis).normal();

	MMatrix swivelMatrix = Maxformations::createRotationMatrix(swivelAngle.asRadians(), 0.0, 0.0, Maxformations::AxisOrder::xyz);
	MMatrix aimMatrix = swivelMatrix * Maxformations::composeMatrix(xAxis, yAxis, zAxis, MPoint(startPoint));

	// Populate matrix array
	//
	MMatrixArray matrices = MMatrixArray(2);
	matrices[0] = aimMatrix;
	matrices[1] = Maxformations::createPositionMatrix(length, 0.0, 0.0) * aimMatrix;

	return matrices;

};


MMatrixArray IKChainControl::solve1Bone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const IKControlSpec& startJoint, const IKControlSpec& endJoint)
/**
Solves a 1-bone system using aim-vector math.
All matrices are in the transform space of the previous joint.
The solution uses the default forward-x and up-y axixes, be sure to use `Maxformations::reorientMatrices` to change this!

@param ikGoal: The IK goal in world space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param startJoint: The start joint in world space.
@param endJoint: The end joint in parent space.
@return: The joint matrices in their respective parent spaces.
*/
{

	return IKChainControl::solve1Bone(ikGoal, upVector, swivelAngle, startJoint, endJoint.length);

};


MMatrixArray IKChainControl::solve2Bone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const IKControlSpec& startJoint, const IKControlSpec& midJoint, const IKControlSpec& endJoint)
/**
Solves a 2-bone system using the law of cosines.
See the following for details: https://www.mathsisfun.com/algebra/trig-solving-sss-triangles.html
All matrices are in the transform space of the previous joint.
The solution uses the default forward-x and up-y axixes, be sure to use `Maxformations::reorientMatrices` to change this!

@param ikGoal: The IK goal in parent space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param startJoint: The start joint in world space.
@param midJoint: The mid joint in parent space.
@param endJoint: The end joint in parent space.
@return: The joint matrices in their respective parent spaces.
*/
{

	// Calculate max limb length
	//
	double startLength = midJoint.length;
	double endLength = endJoint.length;

	double maxDistance = startLength + endLength;

	// Calculate aim vector
	//
	MVector origin = Maxformations::matrixToPosition(startJoint.matrix);
	MVector goalPoint = Maxformations::matrixToPosition(ikGoal);
	MVector aimVector = goalPoint - origin;

	MVector forwardVector = aimVector.normal();
	double distance = aimVector.length();

	// Calculate angles using law of cosines
	//
	double startRadian = 0.0;
	double endRadian = M_PI;

	if (distance < (maxDistance - 1e-3))
	{

		startRadian = acos((pow(startLength, 2.0) + pow(distance, 2.0) - pow(endLength, 2.0)) / (2.0 * startLength * distance));
		endRadian = acos((pow(endLength, 2.0) + pow(startLength, 2.0) - pow(distance, 2.0)) / (2.0 * endLength * startLength));

	}

	// Calculate twist matrix
	// Default orientation is: x = forward, y = up and z = right
	//
	MVector xAxis = forwardVector;
	MVector zAxis = (xAxis ^ upVector).normal();
	MVector yAxis = (zAxis ^ xAxis).normal();

	MMatrix swivelMatrix = Maxformations::createRotationMatrix(swivelAngle.asRadians(), 0.0, 0.0, Maxformations::AxisOrder::xyz);
	MMatrix twistMatrix = swivelMatrix * Maxformations::composeMatrix(xAxis, yAxis, zAxis, MPoint(origin));

	// Compose matrices
	//
	MMatrix startMatrix = Maxformations::createRotationMatrix(0.0, 0.0, startRadian, Maxformations::AxisOrder::xyz) * twistMatrix;
	MMatrix midMatrix = (Maxformations::createRotationMatrix(0.0, 0.0, -(M_PI - endRadian), Maxformations::AxisOrder::xyz) * Maxformations::createPositionMatrix(startLength, 0.0, 0.0) * startMatrix);
	MMatrix endMatrix = Maxformations::createPositionMatrix(endLength, 0.0, 0.0) * midMatrix;

	// Populate matrix array
	//
	MMatrixArray matrices = MMatrixArray(3);
	matrices[0] = startMatrix;
	matrices[1] = midMatrix;
	matrices[2] = endMatrix;

	return matrices;

};


MMatrixArray IKChainControl::solveNBone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const std::vector<IKControlSpec>& joints)
/**
Solves an n-bone system using a weighted angular redistrution solution.
All matrices are in the transform space of the previous joint.
The solution uses the default forward-x and up-y axixes, be sure to use `Maxformations::reorientMatrices` to change this!

@param ikGoal: The IK goal in parent space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param joints: The joints in their respective parent spaces.
@return: The joint matrices in their respective parent spaces.
*/
{

	return MMatrixArray(joints.size());

};


MStatus IKChainControl::legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal)
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
	if ((plug == IKChainControl::ikGoal && asSrc) && otherPlug == PRS::value)
	{

		// Evaluate if other node is a `prs` node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId typeId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = (typeId == PRS::id);

		return MS::kSuccess;

	}
	else if ((plug == IKChainControl::jointMatrix || (plug == IKChainControl::jointPreferredRotationX || plug == IKChainControl::jointPreferredRotationY || plug == IKChainControl::jointPreferredRotationZ)) && asSrc)
	{

		// Evaluate if other node is a `ikControl` node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId typeId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = (typeId == IKControl::id);

		return MS::kSuccess;

	}
	else
	{

		return MPxNode::legalConnection(plug, otherPlug, asSrc, isLegal);

	}

};


MStatus IKChainControl::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == IKChainControl::ikGoal && !asSrc) && otherPlug == PRS::value)
	{

		// Store reference to prs
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		this->prs = static_cast<PRS*>(fnDependNode.userNode());
		CHECK_MSTATUS_AND_RETURN_IT(status);

		this->prs->registerMasterController(this);

	}

	return MPxNode::connectionMade(plug, otherPlug, asSrc);

};


MStatus IKChainControl::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == IKChainControl::ikGoal && !asSrc) && otherPlug == PRS::value)
	{

		// Cleanup reference to prs
		//
		if (this->prs != nullptr)
		{

			this->prs->deregisterMasterController();
			this->prs = nullptr;

		}

	}
	
	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


bool IKChainControl::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return false;

};


void* IKChainControl::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new IKChainControl();

};


MStatus IKChainControl::initialize()
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
	// ".enabled" attribute
	//
	IKChainControl::enabled = fnNumericAttr.create("enabled", "e", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".ikGoal" attribute
	//
	IKChainControl::ikGoal = fnTypedAttr.create("ikGoal", "ikg", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".ikParentMatrix" attribute
	//
	IKChainControl::ikParentMatrix = fnTypedAttr.create("ikParentMatrix", "ipm", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".forwardAxis" attribute
	//
	IKChainControl::forwardAxis = fnEnumAttr.create("forwardAxis", "fa", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField(MString("x"), 0));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("y"), 1));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("z"), 2));

	// ".forwardAxisFlip" attribute
	//
	IKChainControl::forwardAxisFlip = fnNumericAttr.create("forwardAxisFlip", "faf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".upAxis" attribute
	//
	IKChainControl::upAxis = fnEnumAttr.create("upAxis", "ua", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField(MString("x"), 0));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("y"), 1));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("z"), 2));

	// ".upAxisFlip" attribute
	//
	IKChainControl::upAxisFlip = fnNumericAttr.create("upAxisFlip", "uaf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotationX" attribute
	//
	IKChainControl::jointPreferredRotationX = fnUnitAttr.create("jointPreferredRotationX", "jprx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotationY" attribute
	//
	IKChainControl::jointPreferredRotationY = fnUnitAttr.create("jointPreferredRotationY", "jpry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotationZ" attribute
	//
	IKChainControl::jointPreferredRotationZ = fnUnitAttr.create("jointPreferredRotationZ", "jprz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotation" attribute
	//
	IKChainControl::jointPreferredRotation = fnNumericAttr.create("jointPreferredRotation", "jpr", IKChainControl::jointPreferredRotationX, IKChainControl::jointPreferredRotationY, IKChainControl::jointPreferredRotationZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotationX" attribute
	//
	IKChainControl::jointOffsetRotationX = fnUnitAttr.create("jointOffsetRotationX", "jorx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotationY" attribute
	//
	IKChainControl::jointOffsetRotationY = fnUnitAttr.create("jointOffsetRotationY", "jory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotationZ" attribute
	//
	IKChainControl::jointOffsetRotationZ = fnUnitAttr.create("jointOffsetRotationZ", "jorz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotation" attribute
	//
	IKChainControl::jointOffsetRotation = fnNumericAttr.create("jointOffsetRotation", "jor", IKChainControl::jointOffsetRotationX, IKChainControl::jointOffsetRotationY, IKChainControl::jointOffsetRotationZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointMatrix" attribute
	//
	IKChainControl::jointMatrix = fnTypedAttr.create("jointMatrix", "jm", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".joint" attribute
	//
	IKChainControl::joint = fnCompoundAttr.create("joint", "jnt", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(IKChainControl::jointPreferredRotation));
	CHECK_MSTATUS(fnCompoundAttr.addChild(IKChainControl::jointOffsetRotation));
	CHECK_MSTATUS(fnCompoundAttr.addChild(IKChainControl::jointMatrix));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	
	// ".jointParentMatrix" attribute
	//
	IKChainControl::jointParentMatrix = fnTypedAttr.create("jointParentMatrix", "jpm", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".swivelAngle" attribute
	//
	IKChainControl::swivelAngle = fnUnitAttr.create("swivelAngle", "sa", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".useVHTarget" attribute
	//
	IKChainControl::useVHTarget = fnNumericAttr.create("useVHTarget", "uvht", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".vhTarget" attribute
	//
	IKChainControl::vhTarget = fnTypedAttr.create("vhTarget", "vht", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Output attributes:
	// ".goal" attribute
	//
	IKChainControl::goal = fnTypedAttr.create("goal", "g", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.setWritable(false));
	CHECK_MSTATUS(fnTypedAttr.setStorable(false));
	CHECK_MSTATUS(fnTypedAttr.setArray(true));
	CHECK_MSTATUS(fnTypedAttr.setUsesArrayDataBuilder(true));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(IKChainControl::goalCategory));

	// Inherit attributes from parent class
	//
	status = IKChainControl::inheritAttributesFrom("matrix3Controller");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Add attributes to node
	//
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::enabled));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::ikGoal));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::ikParentMatrix));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::forwardAxis));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::forwardAxisFlip));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::upAxis));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::upAxisFlip));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::joint));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::jointParentMatrix));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::swivelAngle));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::useVHTarget));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::vhTarget));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::goal));

	// Define attribute relationships
	//
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::enabled, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::ikGoal, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::ikParentMatrix, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::forwardAxis, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::forwardAxisFlip, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::upAxis, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::upAxisFlip, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::jointPreferredRotation, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::jointOffsetRotation, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::jointMatrix, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::jointParentMatrix, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::swivelAngle, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::useVHTarget, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::vhTarget, IKChainControl::goal));

	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::ikGoal, IKChainControl::value));

	return status;

};