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


IKChainControl::IKChainControl() : Matrix3Controller() {};
IKChainControl::~IKChainControl() {};


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

		bool enabled = enabledHandle.asBool();

		std::vector<IKControlSpec> joints = IKChainControl::getJoints(data, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

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

				upVector = IKChainControl::getUpVector(joints[0].worldMatrix, vhTarget);

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
			MMatrixArray matrices = MMatrixArray(numJoints);
			MMatrixArray offsetMatrices = MMatrixArray(numJoints);

			MMatrix parentMatrix, offsetRotateMatrix;

			for (unsigned int i = 0; i < numJoints; i++)
			{

				offsetRotateMatrix = joints[i].offsetRotation.asMatrix();
				offsetMatrices[i] = offsetRotateMatrix * worldMatrices[i];
				parentMatrix = (i > 0) ? offsetMatrices[i - 1] : joints[0].parentMatrix;

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


std::vector<IKControlSpec> IKChainControl::getJoints(MDataBlock& data, MStatus* status)
/**
Returns an array of ik-control specs from the supplied array handle.

@param arrayHandle: The array data handle to extract from.
@return: The joint values.
*/
{

	// Presize joint array
	//
	std::vector<IKControlSpec> joints = std::vector<IKControlSpec>();

	MArrayDataHandle jointArrayHandle = data.inputArrayValue(IKChainControl::joint, status);
	CHECK_MSTATUS_AND_RETURN(*status, joints);

	unsigned int numElements = jointArrayHandle.elementCount(status);
	CHECK_MSTATUS_AND_RETURN(*status, joints);

	joints.resize(numElements);

	// Get parent matrix
	//
	MDataHandle jointParentMatrixHandle = data.inputValue(IKChainControl::jointParentMatrix, status);
	CHECK_MSTATUS_AND_RETURN(*status, joints);

	MMatrix jointParentMatrix = Maxformations::getMatrixData(jointParentMatrixHandle.data());

	// Iterate through elements
	//
	MDataHandle elementHandle, preferredRotationHandle, offsetRotationHandle, matrixHandle;
	MEulerRotation preferredRotation, offsetRotation;
	MMatrix matrix, parentMatrix, worldMatrix;
	double length = 0.0;

	for (unsigned int i = 0; i < numElements; i++)
	{

		// Go to element in array
		//
		*status = jointArrayHandle.jumpToArrayElement(i);
		CHECK_MSTATUS_AND_RETURN(*status, joints);

		elementHandle = jointArrayHandle.inputValue(status);
		CHECK_MSTATUS_AND_RETURN(*status, joints);

		// Get preferred rotation
		//
		preferredRotationHandle = elementHandle.child(IKChainControl::jointPreferredRotation);
		preferredRotation = MEulerRotation(preferredRotationHandle.asDouble3());

		// Get offset rotation
		//
		offsetRotationHandle = elementHandle.child(IKChainControl::jointOffsetRotation);
		offsetRotation = MEulerRotation(offsetRotationHandle.asDouble3());

		// Get transform matrices
		//
		matrixHandle = elementHandle.child(IKChainControl::jointMatrix);
		matrix = Maxformations::getMatrixData(matrixHandle.data());
		parentMatrix = (i > 0) ? joints[i - 1].worldMatrix : jointParentMatrix;
		worldMatrix = matrix * parentMatrix;
		length = (i > 0) ? Maxformations::distanceBetween(joints[i - 1].worldMatrix, worldMatrix).value() : 0.0;

		// Initialize control specs
		//
		joints[i] = IKControlSpec{ preferredRotation, offsetRotation, matrix, worldMatrix, parentMatrix, length };

	}

	return joints;

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
@return: The best VH target vector.
*/
{

	size_t numJoints = joints.size();

	switch (numJoints)
	{

		case 0: case 1:
		{

			// Default to scene up-vector
			//
			return Maxformations::getSceneUpVector();

		}
		break;

		case 2:
		{

			// Use up-axis vector
			//
			MVector upVector = MVector(joints[0].worldMatrix[upAxis]).normal();
			return upAxisFlip ? -upVector : upVector;

		}
		break;

		case 3:
		{

			// Average up-axis vectors
			//
			MVector startVector = MVector(joints[0].worldMatrix[upAxis]);
			MVector endVector = MVector(joints[1].worldMatrix[upAxis]);
			MVector upVector = ((startVector * 0.5) + (endVector * 0.5)).normal();

			return upAxisFlip ? -upVector : upVector;

		}
		break;

		default:
		{

			return IKChainControl::guessUpVector(joints);

		}
		break;

	}

};


MVector IKChainControl::guessUpVector(const std::vector<IKControlSpec>& joints)
/**
Returns the best up-vector that could be used from the supplied joint matrices.

@param joint: The FK joint chain.
@return: The best VH target vector.
*/
{

	// Check if there are enough joints
	//
	size_t jointCount = joints.size();

	if (!(jointCount >= 3))
	{

		return MVector::zero;

	}

	// Get forward-vector
	//
	size_t lastIndex = jointCount - 1;

	MVector origin = Maxformations::matrixToPosition(joints[0].worldMatrix);
	MVector end = Maxformations::matrixToPosition(joints[lastIndex].worldMatrix);

	MVector forwardVector = (end - origin).normal();

	// Calculate right-vector
	//
	double weight = 1.0 / static_cast<double>(lastIndex);

	MVector point, nextVector, rightVector, average;
	bool flipped;

	for (size_t i = 1; i < jointCount; i++)
	{

		point = Maxformations::matrixToPosition(joints[i].worldMatrix);
		nextVector = (point - origin).normal();

		rightVector = (forwardVector ^ nextVector).normal();
		
		if (i == 1)
		{

			average += (rightVector * weight);

		}
		else
		{

			flipped = (rightVector * average) < 0.0;
			average += flipped ? -(rightVector * weight) : (rightVector * weight);

		}

	}

	// Calculate up-vector
	//
	MVector upVector = (average.normal() ^ forwardVector).normal();

	return upVector;

};


MMatrixArray IKChainControl::solve(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const std::vector<IKControlSpec>& joints)
/**
Returns an IK solution for the supplied joint chain.
The solution uses the default forward-x and up-y axixes in world-space!

@param ikGoal: The IK goal in world space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param joints: The joints in their respective parent spaces.
@return: The IK solution.
*/
{

	unsigned int numItems = static_cast<unsigned int>(joints.size());

	switch (numItems)
	{

	case 0: case 1:
		return MMatrixArray();

	case 2:
		return IKChainControl::solve1Bone(ikGoal, upVector, swivelAngle, joints[0], joints[1]);

	case 3:
		return IKChainControl::solve2Bone(ikGoal, upVector, swivelAngle, joints[0], joints[1], joints[2]);

	default:
		return IKChainControl::solveNBone(ikGoal, upVector, swivelAngle, joints, joints.size());

	}

};


MMatrixArray IKChainControl::solve1Bone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const IKControlSpec& startJoint, const IKControlSpec& endJoint)
/**
Solves a 1-bone system using aim-vector math.
All matrices are in the transform space of the previous joint.
The solution uses the default forward-x and up-y axixes in world-space!

@param ikGoal: The IK goal in world space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param startJoint: The start joint specs.
@param endJoint: The end joint specs.
@return: The IK solution.
*/
{

	// Calculate forward vector
	//
	MVector startPoint = Maxformations::matrixToPosition(startJoint.worldMatrix);
	MVector goalPoint = Maxformations::matrixToPosition(ikGoal);

	MVector aimVector = goalPoint - startPoint;
	MVector forwardVector = aimVector.normal();

	// Calculate axis vectors
	//
	MVector xAxis = forwardVector;
	MVector zAxis = (xAxis ^ upVector).normal();
	MVector yAxis = (zAxis ^ xAxis).normal();

	MMatrix swivelMatrix = Maxformations::createRotationMatrix(swivelAngle.asRadians(), 0.0, 0.0, Maxformations::AxisOrder::xyz);
	MMatrix aimMatrix = Maxformations::composeMatrix(xAxis, yAxis, zAxis, MPoint(startPoint));

	MMatrix startMatrix = swivelMatrix * aimMatrix;
	MMatrix endMatrix = Maxformations::createPositionMatrix(endJoint.length, 0.0, 0.0) * startMatrix;

	// Populate matrix array
	//
	MMatrixArray matrices = MMatrixArray(2);
	matrices[0] = Maxformations::createScaleMatrix(startJoint.worldMatrix) * startMatrix;
	matrices[1] = Maxformations::createScaleMatrix(endJoint.worldMatrix) * endMatrix;

	return matrices;

};


MMatrixArray IKChainControl::solve2Bone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const IKControlSpec& startJoint, const IKControlSpec& midJoint, const IKControlSpec& endJoint)
/**
Solves a 2-bone system using the law of cosines.
See the following for details: https://www.mathsisfun.com/algebra/trig-solving-sss-triangles.html
The solution uses the default forward-x and up-y axixes in world-space!

@param ikGoal: The IK goal in parent space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param startJoint: The start joint specs.
@param midJoint: The mid joint specs.
@param endJoint: The end joint specs.
@return: The IK solution.
*/
{

	// Calculate min/max limb length
	//
	double startLength = midJoint.length;
	double endLength = endJoint.length;

	double maxDistance = startLength + endLength;
	double minDistance = std::fabs(endLength - startLength);

	// Calculate aim vector
	//
	MVector origin = Maxformations::matrixToPosition(startJoint.worldMatrix);
	MVector goalPoint = Maxformations::matrixToPosition(ikGoal);
	MVector aimVector = goalPoint - origin;

	MVector forwardVector = aimVector.normal();
	double distance = aimVector.length();

	// Calculate angles using law of cosines
	//
	double startRadian = 0.0;
	double endRadian = 0.0;

	if (distance < (minDistance + 1e-3)) // Collapsed
	{

		endRadian = 0.0; 

	}
	else if (distance > (maxDistance - 1e-3))  // Hyper-extended
	{

		endRadian = M_PI;

	}
	else  // Default
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
	MMatrix midMatrix = Maxformations::createRotationMatrix(0.0, 0.0, -(M_PI - endRadian), Maxformations::AxisOrder::xyz) * Maxformations::createPositionMatrix(startLength, 0.0, 0.0) * startMatrix;
	MMatrix endMatrix = Maxformations::createPositionMatrix(endLength, 0.0, 0.0) * midMatrix;

	// Populate matrix array
	//
	MMatrixArray matrices = MMatrixArray(3);
	matrices[0] = Maxformations::createScaleMatrix(startJoint.worldMatrix) * startMatrix;
	matrices[1] = Maxformations::createScaleMatrix(midJoint.worldMatrix) * midMatrix;
	matrices[2] = Maxformations::createScaleMatrix(endJoint.worldMatrix) * endMatrix;

	return matrices;

};


double IKChainControl::lagrange2d(const double x, const MVector& p1, const MVector& p2, const MVector& p3)
/**
Returns the result of a lagrange2d polynomial that goes through the specified points.
See the following: https://www.desmos.com/calculator/vx6lr2w13j

@param x: The value to interpolate
@param p1: The first 2d point.
@param p2: The second 2d point.
@param p3: The third 2d point.
@return: The interpolated value.
*/
{

	double a = p3.y / ((p3.x - p1.x) * (p3.x - p2.x));
	double b = p2.y / ((p2.x - p1.x) * (p2.x - p3.x));
	double c = p1.y / ((p1.x - p2.x) * (p1.x - p3.x));

	return a * (x - p1.x) * (x - p2.x) + b * (x - p1.x) * (x - p3.x) + c * (x - p2.x) * (x - p3.x);

};


MPointArray IKChainControl::compressPoints(const MPointArray& points, const MVector& goal)
/**
Compresses the supplied points based on the distance change.

@param points: The points to compress.
@param effector: The current effector position.
@return: Compressed points.
*/
{

	// Redundancy check
	//
	unsigned int pointCount = points.length();

	if (!(pointCount >= 3))
	{

		return points;  // Minimum of 3 points required!

	}

	// Get bone specs from points
	//
	unsigned int boneCount = pointCount - 1;
	unsigned int angleCount = pointCount - 2;

	MDoubleArray lengths = MDoubleArray(boneCount);
	MDoubleArray angles = MDoubleArray(angleCount);
	
	MVector startVector, endVector, rightVector;
	double startLength, endLength;
	double angle = 0.0, angleSum = 0.0;
	double minDistance = 0.0, maxDistance = 0.0;
	bool flipped = false, alternates = false;

	for (unsigned int i = 0, j = 1, k = 2; k < pointCount; i++, j++, k++)
	{

		startVector = (points[i] - points[j]);
		endVector = (points[k] - points[j]);

		startLength = startVector.length();
		endLength = endVector.length();
		lengths[i] = startLength, lengths[j] = endLength;
		minDistance = (i > 0) ? fabs(endLength - minDistance) : fabs(startLength - endLength);
		maxDistance += (i > 0) ? endLength : startLength + endLength;

		angle = startVector.angle(endVector);
		angles[i] = angle;
		angleSum += angle;

		rightVector = (i == 0) ? (startVector ^ endVector).normal() : rightVector;
		flipped = (i > 0) ? ((startVector ^ endVector).normal() * rightVector) <= 0.0 : false;
		alternates = (flipped) ? true : alternates;

	}

	double minAngleSum = alternates ? 0.0 : ((double)boneCount - 2.0) * M_PI;
	double maxAngleSum = M_PI * (double)angleCount;

	// Compute angle weights
	//
	MDoubleArray weights = MDoubleArray(angleCount);

	for (unsigned int i = 0; i < angleCount; i++)
	{

		weights[i] = angles[i] / angleSum;

	}

	// Compute new weighted angle sum
	//
	unsigned int lastIndex = pointCount - 1;
	MPoint startPoint = points[0];
	MPoint endPoint = points[lastIndex];

	double distance = startPoint.distanceTo(endPoint);
	double goalDistance = startPoint.distanceTo(goal);

	double weightedAngleSum = 0.0;

	if (goalDistance <= minDistance)
	{

		weightedAngleSum = minAngleSum;

	}
	else if (goalDistance >= maxDistance)
	{

		weightedAngleSum = maxAngleSum;

	}
	else
	{

		weightedAngleSum = IKChainControl::lagrange2d(goalDistance, MVector(minDistance, minAngleSum), MVector(distance, angleSum), MVector(maxDistance, maxAngleSum));  // x = distance; y = angle sum
	
	}

	// Adjust points using weighted angles
	//
	MPointArray weightedPoints = MPointArray(points);
	MVector restStartVector, restEndVector, axisVector, rotatedVector;
	MQuaternion rotation;
	double weightedAngle;

	for (unsigned int i = 0, j = 1, k = 2; k < pointCount; i++, j++, k++)
	{

		// Derive rotation axis from vector pairs
		//
		restStartVector = (points[i] - points[j]).normal();
		restEndVector = (points[k] - points[j]).normal();

		axisVector = (restStartVector ^ restEndVector).normal();

		// Check if start vector requires counter rotating
		//
		weightedAngle = weightedAngleSum * weights[i];
		
		if (i == 0)
		{
			
			startVector = (weightedPoints[j] - weightedPoints[i]).normal();
			rotation = MQuaternion((weightedAngle - angles[i]), -axisVector);
			rotatedVector = startVector.rotateBy(rotation);

			weightedPoints[j] = weightedPoints[i] + (rotatedVector * lengths[i]);
			
		}
		
		// Apply weighted rotation to vector
		//
		startVector = (weightedPoints[i] - weightedPoints[j]).normal();
		rotation = MQuaternion(weightedAngle, axisVector);
		rotatedVector = startVector.rotateBy(rotation);

		weightedPoints[k] = weightedPoints[j] + (rotatedVector * lengths[j]);

	}

	return weightedPoints;

};


MMatrixArray IKChainControl::solveNBone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const std::vector<IKControlSpec>& joints, const unsigned int iterations)
/**
Solves an n-bone system using a FABRIK solver.
The solution uses the default forward-x and up-y axixes in world-space!

@param ikGoal: The IK goal in parent space.
@param upVector: The up vector to orient the joint chain.
@param swivelAngle: The twist value along the aim vector.
@param joints: The joint specs.
@return: The IK solution.
*/
{

	// Collect points from joint chain
	//
	size_t jointCount = joints.size();
	size_t lastIndex = jointCount - 1;

	MMatrixArray matrices = MMatrixArray(jointCount);  // This will serve as our fallback!
	MPointArray points = MPointArray(jointCount);
	double chainLength = 0.0;

	for (size_t i = 0; i < jointCount; i++)
	{

		matrices[i] = joints[i].worldMatrix;
		points[i] = Maxformations::matrixToPosition(matrices[i]);

		chainLength += joints[i].length;

	}
	
	MVector origin = points[0];
	MVector goal = Maxformations::matrixToPosition(ikGoal);

	MPointArray adjustedPoints = IKChainControl::compressPoints(points, goal);  // This will give us a better start pose!

	// Calculate alternate goal relative to start/end
	//
	MVector aimVector = goal - origin;
	MVector normalizedAimVector = aimVector.normal();
	double aimLength = aimVector.length();

	MVector tip = adjustedPoints[lastIndex];
	MVector tipVector = (tip - origin).normal();

	MVector altGoal = origin + (tipVector * aimLength);

	// Apply FABRIK to points
	//
	MPointArray previousPoints = MPointArray(adjustedPoints);
	MPointArray nextPoints = MPointArray(adjustedPoints);

	size_t headIndex, tailIndex;
	MPoint headPoint, tailPoint, effectorPoint;
	double length;
	
	for (unsigned int i = 0; i < iterations; i++)
	{

		// Backwards solve
		//
		for (size_t j = lastIndex; j > 0; j--)
		{

			headIndex = j;
			tailIndex = j - 1;

			headPoint = previousPoints[headIndex];
			tailPoint = previousPoints[tailIndex];
			effectorPoint = (headIndex == lastIndex) ? altGoal : nextPoints[headIndex];

			length = (headPoint - tailPoint).length();
			nextPoints[headIndex] = effectorPoint;
			nextPoints[tailIndex] = effectorPoint + ((tailPoint - effectorPoint).normal() * length);

		}

		previousPoints = MPointArray(nextPoints);
		
		// Forwards solve
		//
		for (size_t j = 0; j < lastIndex; j++)
		{

			headIndex = j;
			tailIndex = j + 1;

			headPoint = previousPoints[headIndex];
			tailPoint = previousPoints[tailIndex];
			effectorPoint = (headIndex == 0) ? origin : nextPoints[headIndex];

			length = (headPoint - tailPoint).length();
			nextPoints[headIndex] = effectorPoint;
			nextPoints[tailIndex] = effectorPoint + ((tailPoint - effectorPoint).normal() * length);

		}

		previousPoints = MPointArray(nextPoints);

	}
	
	// Compose aim matrices from points
	//
	MVector restUpVector = IKChainControl::guessUpVector(joints);
	MVector restRightVector = (normalizedAimVector ^ restUpVector).normal();

	MMatrixArray aimMatrices;

	MStatus status = Maxformations::createAimMatrix(previousPoints, 0, false, restRightVector, 2, false, aimMatrices);
	CHECK_MSTATUS_AND_RETURN(status, matrices);

	// Reorient aim matrices
	//
	MMatrix restMatrix, offsetMatrix;

	status = Maxformations::createAimMatrix(tipVector, 0, restUpVector, 1, origin, restMatrix);
	CHECK_MSTATUS_AND_RETURN(status, matrices);

	status = Maxformations::createAimMatrix(normalizedAimVector, 0, upVector, 1, origin, offsetMatrix);
	CHECK_MSTATUS_AND_RETURN(status, matrices);

	MMatrix swivelMatrix = Maxformations::createRotationMatrix(swivelAngle.asRadians(), 0.0, 0.0, Maxformations::AxisOrder::xyz);

	for (size_t i = 0; i < jointCount; i++)
	{

		matrices[i] = (aimMatrices[i] * restMatrix.inverse()) * (swivelMatrix * offsetMatrix);

	}

	return matrices;
	
};


MStatus IKChainControl::legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal)
/**
This method allows you to check for legal connections being made to attributes of this node.
You should return `kUnknownParameter` to specify that maya should handle this connection if you are unable to determine if it is legal.

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

		// Evaluate if other node is supported
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isPRS = Maxformations::hasTypeId(otherNode, PRS::id, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = isPRS;

		return MS::kSuccess;

	}
	else if ((plug == IKChainControl::jointMatrix || (plug == IKChainControl::jointPreferredRotationX || plug == IKChainControl::jointPreferredRotationY || plug == IKChainControl::jointPreferredRotationZ)) && asSrc)
	{

		// Evaluate if other node is supported
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isIKControl = Maxformations::hasTypeId(otherNode, IKControl::id, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = isIKControl;

		return MS::kSuccess;

	}
	else
	{

		return MPxNode::legalConnection(plug, otherPlug, asSrc, isLegal);

	}

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