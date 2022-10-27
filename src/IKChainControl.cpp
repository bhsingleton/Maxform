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
MObject	IKChainControl::forwardAxis;
MObject	IKChainControl::forwardAxisFlip;
MObject	IKChainControl::upAxis;
MObject	IKChainControl::upAxisFlip;
MObject	IKChainControl::joint;
MObject	IKChainControl::jointPreferredRotation;
MObject	IKChainControl::jointPreferredRotationX;
MObject	IKChainControl::jointPreferredRotationY;
MObject	IKChainControl::jointPreferredRotationZ;
MObject	IKChainControl::jointMatrix;
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

	bool isValue = fnAttribute.hasCategory(Matrix3Controller::valueCategory);
	bool isGoal = fnAttribute.hasCategory(IKChainControl::goalCategory);

	if (isValue)
	{

		// Copy value from data handle
		//
		MDataHandle ikGoalHandle = data.inputValue(IKChainControl::ikGoal, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueHandle = data.outputValue(IKChainControl::value, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		valueHandle.setMMatrix(ikGoalHandle.asMatrix());

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

		bool enabled = enabledHandle.asBool();

		if (enabled)
		{

			// Get input handles
			//
			MDataHandle ikGoalHandle = data.inputValue(IKChainControl::ikGoal, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle forwardAxisHandle = data.inputValue(IKChainControl::forwardAxis, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle forwardAxisFlipHandle = data.inputValue(IKChainControl::forwardAxisFlip, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle upAxisHandle = data.inputValue(IKChainControl::upAxis, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle upAxisFlipHandle = data.inputValue(IKChainControl::upAxisFlip, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MArrayDataHandle jointArrayHandle = data.inputArrayValue(IKChainControl::joint, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle swivelAngleHandle = data.inputValue(IKChainControl::swivelAngle, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle useVHTargetHandle = data.inputValue(IKChainControl::useVHTarget, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle vhTargetHandle = data.inputValue(IKChainControl::vhTarget, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MMatrix ikGoal = ikGoalHandle.asMatrix();
			int forwardAxis = forwardAxisHandle.asInt();
			bool forwardAxisFlip = forwardAxisFlipHandle.asBool();
			int upAxis = upAxisHandle.asInt();
			bool upAxisFlip = upAxisFlipHandle.asBool();
			std::vector<JointItem> joints = IKChainControl::getJoints(jointArrayHandle);
			MAngle swivelAngle = swivelAngleHandle.asAngle();
			bool usVHTarget = useVHTargetHandle.asBool();
			MMatrix vhTarget = vhTargetHandle.asMatrix();

			// Solve IK system
			//
			MMatrixArray matrices = IKChainControl::solve(ikGoal, vhTarget, swivelAngle, joints);

			if ((forwardAxis != 0 || !forwardAxisFlip) || upAxis != 1 || !upAxisFlip)
			{

				matrices = Maxformations::reorientMatrices(matrices, forwardAxis, forwardAxisFlip, upAxis, upAxisFlip);

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

				goalHandle.setMMatrix(matrices[i]);
				goalHandle.setClean();

			}

			status = goalArrayHandle.set(builder);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		else
		{

			// Copy values between array handles
			//
			MArrayDataHandle jointArrayHandle = data.inputArrayValue(IKChainControl::joint, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			unsigned int numElements = jointArrayHandle.elementCount(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MArrayDataHandle goalArrayHandle = data.outputArrayValue(IKChainControl::goal, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MArrayDataBuilder builder = goalArrayHandle.builder(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle jointHandle, jointMatrixHandle, goalHandle;

			for (unsigned int i = 0; i < numElements; i++)
			{

				status = jointArrayHandle.jumpToArrayElement(i);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				jointHandle = jointArrayHandle.inputValue(&status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				jointMatrixHandle = jointHandle.child(IKChainControl::jointMatrix);

				goalHandle = builder.addElement(i, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				goalHandle.copy(jointMatrixHandle);
				goalHandle.setClean();

			}

			status = goalArrayHandle.set(builder);
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


std::vector<JointItem> IKChainControl::getJoints(MArrayDataHandle& arrayHandle)
/**
Returns an array of joint items from the supplied array handle.

@param arrayHandle: The array data handle to extract from.
@return: The joint values.
*/
{

	MStatus status;

	// Presize joint array
	//
	unsigned int numElements = arrayHandle.elementCount();
	std::vector<JointItem> joints = std::vector<JointItem>(numElements);

	// Iterate through elements
	//
	MDataHandle elementHandle, preferredRotationHandle, matrixHandle;
	MMatrix matrix;

	for (unsigned int i = 0; i < numElements; i++)
	{

		status = arrayHandle.jumpToArrayElement(i);
		CHECK_MSTATUS_AND_RETURN(status, joints);

		elementHandle = arrayHandle.inputValue(&status);
		CHECK_MSTATUS_AND_RETURN(status, joints);

		preferredRotationHandle = elementHandle.child(IKChainControl::jointPreferredRotation);
		matrixHandle = elementHandle.child(IKChainControl::jointMatrix);

		joints[i] = JointItem{ MEulerRotation(preferredRotationHandle.asDouble3()), matrixHandle.asMatrix() };

	}

	return joints;

};


MMatrix IKChainControl::guessVHTarget(const std::vector<JointItem>& joints, const int upAxis, const bool upAxisFlip)
/**
Returns the best VH target that could be used for the supplied joint matrices.

@param joint: The FK joint chain.
@param upAxis: The up-axis for the joint chain.
@param upAxisFlip: Determines if the up-axis is flipped.
@return: The best VH target matrix.
*/
{

	return MMatrix::identity;

};


MMatrixArray IKChainControl::solve(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const std::vector<JointItem>& joints)
/**
Returns a solution for the supplied joint matrices.
The solution uses the default forward-x and up-y axixes!

@param ikGoal: The IK goal in parent space.
@param vhTarget: The pole matrix in ik-handle's parent space.
@param swivelAngle: The twist value along the aim vector.
@param joints: The joints in their respective parent spaces.
@return: The joint matrices in their respective parent spaces.
*/
{

	unsigned int numItems = static_cast<unsigned int>(joints.size());

	switch (numItems)
	{

		case 0: case 1:
			return MMatrixArray();

		case 2:
			return IKChainControl::solve1Bone(ikGoal, vhTarget, swivelAngle, joints[0], joints[1]);
		
		case 3:
			return IKChainControl::solve2Bone(ikGoal, vhTarget, swivelAngle, joints[0], joints[1], joints[2]);

		default:
			return IKChainControl::solveNBone(ikGoal, vhTarget, swivelAngle, joints);

	}

};


MMatrixArray IKChainControl::solve1Bone(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const JointItem& startJoint, const JointItem& endJoint)
/**
Solves a 1-bone system using vector math.
The solution uses the default forward-x and up-y axixes!

@param ikGoal: The IK goal in parent space.
@param vhTarget: The pole matrix in ik-handle's parent space.
@param swivelAngle: The twist value along the aim vector.
@param startJoint: The start joint in parent space.
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

	// Calculate up vector
	//
	MVector vhPoint = Maxformations::matrixToPosition(vhTarget);
	MVector upVector = (vhPoint - startPoint).normal();

	// Calculate axis vectors
	//
	MVector xAxis = forwardVector;
	MVector zAxis = (upVector ^ xAxis).normal();
	MVector yAxis = (xAxis ^ zAxis).normal();

	MMatrix swivelMatrix = Maxformations::createRotationMatrix(swivelAngle.asRadians(), 0.0, 0.0, Maxformations::AxisOrder::xyz);
	MMatrix aimMatrix = swivelMatrix * Maxformations::createMatrix(xAxis, yAxis, zAxis, MPoint(startPoint));

	// Calculate bone length
	//
	MVector endPoint = Maxformations::matrixToPosition(endJoint.matrix);
	double length = endPoint.length();

	// Populate matrix array
	//
	MMatrixArray matrices = MMatrixArray(2);
	matrices[0] = aimMatrix;
	matrices[1] = Maxformations::createPositionMatrix(length, 0.0, 0.0) * aimMatrix;

	return Maxformations::staggerMatrices(matrices);

};


MMatrixArray IKChainControl::solve2Bone(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const JointItem& startJoint, const JointItem& midJoint, const JointItem& endJoint)
/**
Solves a 2-bone system using the law of cosines.
See the following for details: https://www.mathsisfun.com/algebra/trig-solving-sss-triangles.html
The solution uses the default forward-x and up-y axixes!

@param ikGoal: The IK goal in parent space.
@param vhTarget: The pole matrix in ik-handle's parent space.
@param swivelAngle: The twist value along the aim vector.
@param startJoint: The start joint in parent space.
@param midJoint: The mid joint in parent space.
@param endJoint: The end joint in parent space.
@return: The joint matrices in their respective parent spaces.
*/
{

	// Get limb points
	//
	MVector startPoint = Maxformations::matrixToPosition(startJoint.matrix);
	MVector midPoint = Maxformations::matrixToPosition(midJoint.matrix);
	MVector endPoint = Maxformations::matrixToPosition(endJoint.matrix);

	// Calculate limb length
	//
	double startLength = midPoint.length();
	double endLength = endPoint.length();

	double maxLength = startLength + endLength;

	// Calculate aim vector
	//
	MVector goalPoint = Maxformations::matrixToPosition(ikGoal);
	MVector aimVector = goalPoint - startPoint;
	MVector normalizedAimVector = aimVector.normal();

	double distance = aimVector.length();
	double clampedDistance = (distance >= maxLength) ? maxLength : distance;

	// Calculate up vector
	//
	MVector vhPoint = Maxformations::matrixToPosition(vhTarget);
	MVector upVector = (vhPoint - startPoint).normal();

	// Calculate angles using law of cosines
	//
	double startRadian = acos((pow(startLength, 2.0) + pow(clampedDistance, 2.0) - pow(endLength, 2.0)) / (2.0 * startLength * clampedDistance));
	double endRadian = acos((pow(endLength, 2.0) + pow(startLength, 2.0) - pow(clampedDistance, 2.0)) / (2.0 * endLength * startLength));

	// Calculate twist matrix
	//
	MVector xAxis = normalizedAimVector;
	MVector zAxis = (upVector ^ normalizedAimVector).normal();
	MVector yAxis = (xAxis ^ zAxis).normal();

	MMatrix swivelMatrix = Maxformations::createRotationMatrix(swivelAngle.asRadians(), 0.0, 0.0, Maxformations::AxisOrder::xyz);
	MMatrix twistMatrix = swivelMatrix * Maxformations::createMatrix(xAxis, yAxis, zAxis, MPoint(startPoint));

	// Compose matrices
	//
	MMatrix startMatrix = Maxformations::createRotationMatrix(0.0, 0.0, startRadian, Maxformations::AxisOrder::xyz) * twistMatrix;
	MMatrix midMatrix = Maxformations::createPositionMatrix(startLength, 0.0, 0.0) * (Maxformations::createRotationMatrix(0.0, 0.0, -(M_PI - endRadian), Maxformations::AxisOrder::xyz) * startMatrix);
	MMatrix endMatrix = Maxformations::createPositionMatrix(endLength, 0.0, 0.0) * midMatrix;

	// Populate matrix array
	//
	MMatrixArray matrices = MMatrixArray(3);
	matrices[0] = startMatrix;
	matrices[1] = midMatrix;
	matrices[2] = endMatrix;

	return Maxformations::staggerMatrices(matrices);

};


MMatrixArray IKChainControl::solveNBone(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const std::vector<JointItem>& joints)
/**
Solves an n-bone system using a weighed angular redistrution method.
The solution uses the default forward-x and up-y axixes!

@param ikGoal: The IK goal in parent space.
@param vhTarget: The pole matrix in ik-handle's parent space.
@param swivelAngle: The twist value along the aim vector.
@param joints: The joints in their respective parent spaces.
@return: The joint matrices in their respective parent spaces.
*/
{

	unsigned int numElements = joints.size();
	MMatrixArray matrices = MMatrixArray(numElements);

	return matrices;

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
	if (plug == IKChainControl::ikGoal && asSrc)
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
	if (plug == IKChainControl::ikGoal && !asSrc)
	{

		// Inspect other node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId otherId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (otherId == PRS::id)
		{

			this->prs = static_cast<PRS*>(fnDependNode.userNode());

		}

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
	if ((plug == IKChainControl::ikGoal && !asSrc) && this->prs != nullptr)
	{

		this->prs = nullptr;

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
	IKChainControl::ikGoal = fnMatrixAttr.create("ikGoal", "ikg", MFnMatrixAttribute::kDouble, &status);
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

	// ".jointMatrix" attribute
	//
	IKChainControl::jointMatrix = fnMatrixAttr.create("jointMatrix", "jm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".joint" attribute
	//
	IKChainControl::joint = fnCompoundAttr.create("joint", "jnt", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(IKChainControl::jointPreferredRotation));
	CHECK_MSTATUS(fnCompoundAttr.addChild(IKChainControl::jointMatrix));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));

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
	IKChainControl::vhTarget = fnMatrixAttr.create("vhTarget", "vht", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Output attributes:
	// ".goal" attribute
	//
	IKChainControl::goal = fnMatrixAttr.create("goal", "g", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.setArray(true));
	CHECK_MSTATUS(fnMatrixAttr.setUsesArrayDataBuilder(true));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(IKChainControl::goalCategory));

	// Inherit attributes from parent class
	//
	status = IKChainControl::inheritAttributesFrom("matrix3Controller");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Add attributes to node
	//
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::enabled));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::ikGoal));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::forwardAxis));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::forwardAxisFlip));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::upAxis));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::upAxisFlip));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::joint));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::swivelAngle));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::useVHTarget));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::vhTarget));
	CHECK_MSTATUS(IKChainControl::addAttribute(IKChainControl::goal));

	// Define attribute relationships
	//
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::enabled, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::ikGoal, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::forwardAxis, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::forwardAxisFlip, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::upAxis, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::upAxisFlip, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::jointPreferredRotation, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::jointMatrix, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::swivelAngle, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::useVHTarget, IKChainControl::goal));
	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::vhTarget, IKChainControl::goal));

	CHECK_MSTATUS(IKChainControl::attributeAffects(IKChainControl::ikGoal, IKChainControl::value));

	return status;

};