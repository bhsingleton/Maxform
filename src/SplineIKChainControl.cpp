//
// File: SplineIKChainControl.cpp
//
// Dependency Graph Node: splineIKChainControl
//
// Author: Benjamin H. Singleton
//

#include "SplineIKChainControl.h"

MObject	SplineIKChainControl::enabled;
MObject	SplineIKChainControl::ikGoal;
MObject SplineIKChainControl::ikParentMatrix;
MObject	SplineIKChainControl::forwardAxis;
MObject	SplineIKChainControl::forwardAxisFlip;
MObject	SplineIKChainControl::upAxis;
MObject	SplineIKChainControl::upAxisFlip;
MObject	SplineIKChainControl::joint;
MObject	SplineIKChainControl::jointPreferredRotation;
MObject	SplineIKChainControl::jointPreferredRotationX;
MObject	SplineIKChainControl::jointPreferredRotationY;
MObject	SplineIKChainControl::jointPreferredRotationZ;
MObject	SplineIKChainControl::jointOffsetRotation;
MObject	SplineIKChainControl::jointOffsetRotationX;
MObject	SplineIKChainControl::jointOffsetRotationY;
MObject	SplineIKChainControl::jointOffsetRotationZ;
MObject	SplineIKChainControl::jointMatrix;
MObject	SplineIKChainControl::jointParentMatrix;
MObject	SplineIKChainControl::splineShape;
MObject	SplineIKChainControl::startTwistAngle;
MObject	SplineIKChainControl::endTwistAngle;
MObject	SplineIKChainControl::useUpNode;
MObject	SplineIKChainControl::upNode;
MObject	SplineIKChainControl::iterations;
MObject	SplineIKChainControl::tolerance;

MObject	SplineIKChainControl::goal;

MString	SplineIKChainControl::inputCategory("Input");
MString	SplineIKChainControl::goalCategory("Goal");

MTypeId	SplineIKChainControl::id(0x0013b1d3);


SplineIKChainControl::SplineIKChainControl() : Matrix3Controller() { this->iterationLimit = 10; this->toleranceValue = 1e-3; this->prs = nullptr; };
SplineIKChainControl::~SplineIKChainControl() { this->prs = nullptr; };


MStatus SplineIKChainControl::compute(const MPlug& plug, MDataBlock& data)
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

	bool isValue = fnAttribute.hasCategory(SplineIKChainControl::valueCategory);
	bool isGoal = fnAttribute.hasCategory(SplineIKChainControl::goalCategory);
	
	if (isValue)
	{

		// Copy value from data handle
		//
		MDataHandle ikGoalHandle = data.inputValue(SplineIKChainControl::ikGoal, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueHandle = data.outputValue(SplineIKChainControl::value, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		valueHandle.setMObject(ikGoalHandle.data());

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
		MDataHandle enabledHandle = data.inputValue(SplineIKChainControl::enabled, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool enabled = enabledHandle.asBool();

		MArrayDataHandle jointArrayHandle = data.inputArrayValue(SplineIKChainControl::joint, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		std::vector<IKControlSpec> joints = SplineIKChainControl::getJoints(jointArrayHandle);
		unsigned int numJoints = static_cast<unsigned int>(joints.size());

		if (enabled && numJoints >= 2)
		{
			
			// Modify transform space on start joint
			//
			MDataHandle jointParentMatrixHandle = data.inputValue(SplineIKChainControl::jointParentMatrix, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MMatrix jointParentMatrix = Maxformations::getMatrixData(jointParentMatrixHandle.data());

			joints[0].matrix *= jointParentMatrix;

			// Get input handles
			//
			MDataHandle ikParentMatrixHandle = data.inputValue(SplineIKChainControl::ikParentMatrix, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle forwardAxisHandle = data.inputValue(SplineIKChainControl::forwardAxis, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle forwardAxisFlipHandle = data.inputValue(SplineIKChainControl::forwardAxisFlip, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle upAxisHandle = data.inputValue(SplineIKChainControl::upAxis, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle upAxisFlipHandle = data.inputValue(SplineIKChainControl::upAxisFlip, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle splineShapeHandle = data.inputValue(SplineIKChainControl::splineShape, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle startTwistAngleHandle = data.inputValue(SplineIKChainControl::startTwistAngle, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle endTwistAngleHandle = data.inputValue(SplineIKChainControl::endTwistAngle, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle useUpNodeHandle = data.inputValue(SplineIKChainControl::useUpNode, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MDataHandle upNodeHandle = data.inputValue(SplineIKChainControl::upNode, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get ik system settings
			//
			int forwardAxis = forwardAxisHandle.asShort();
			bool forwardAxisFlip = forwardAxisFlipHandle.asBool();
			int upAxis = upAxisHandle.asShort();
			bool upAxisFlip = upAxisFlipHandle.asBool();
			MObject splineShape = splineShapeHandle.asNurbsCurve();
			MAngle startTwistAngle = startTwistAngleHandle.asAngle();
			MAngle endTwistAngle = endTwistAngleHandle.asAngle();

			// Get vector-handle target
			//
			bool useUpNode = useUpNodeHandle.asBool();
			MVector upVector;

			if (useUpNode)
			{

				MMatrix upMatrix = Maxformations::getMatrixData(upNodeHandle.data());
				upVector = MVector(upMatrix[2]).normal();  // Max always uses the z axis!!!

			}
			else
			{

				upVector = Maxformations::getSceneUpVector();

			}

			// Solve ik system
			//
			MMatrixArray worldMatrices;

			status = SplineIKChainControl::solve(splineShape, upVector, startTwistAngle, endTwistAngle, joints, worldMatrices);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			if ((forwardAxis != 0 || !forwardAxisFlip) || upAxis != 1 || !upAxisFlip)
			{

				status = Maxformations::reorientMatrices(worldMatrices, forwardAxis, forwardAxisFlip, upAxis, upAxisFlip);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

			// Localize solution back into parent space
			//
			MMatrixArray matrices = MMatrixArray(numJoints);
			MMatrix parentMatrix;

			for (unsigned int i = 0; i < numJoints; i++)
			{

				parentMatrix = (i == 0) ? jointParentMatrix : worldMatrices[i - 1];
				matrices[i] = (joints[i].offsetRotation.asMatrix() * worldMatrices[i]) * parentMatrix.inverse();

			}

			// Update output handles
			//
			MArrayDataHandle goalArrayHandle = data.outputArrayValue(SplineIKChainControl::goal, &status);
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
			MArrayDataHandle goalArrayHandle = data.outputArrayValue(SplineIKChainControl::goal, &status);
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


std::vector<IKControlSpec> SplineIKChainControl::getJoints(MArrayDataHandle& arrayHandle)
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
	double length, distance = 0.0;

	for (unsigned int i = 0; i < numElements; i++)
	{

		status = arrayHandle.jumpToArrayElement(i);
		CHECK_MSTATUS_AND_RETURN(status, joints);

		elementHandle = arrayHandle.inputValue(&status);
		CHECK_MSTATUS_AND_RETURN(status, joints);

		preferredRotationHandle = elementHandle.child(SplineIKChainControl::jointPreferredRotation);
		preferredRotation = MEulerRotation(preferredRotationHandle.asDouble3());

		offsetRotationHandle = elementHandle.child(SplineIKChainControl::jointOffsetRotation);
		offsetRotation = MEulerRotation(offsetRotationHandle.asDouble3());

		matrixHandle = elementHandle.child(SplineIKChainControl::jointMatrix);
		matrix = Maxformations::getMatrixData(matrixHandle.data());
		length = i > 0 ? Maxformations::matrixToPosition(matrix).length() : 0.0;
		distance += length;

		joints[i] = IKControlSpec{ preferredRotation, offsetRotation, matrix, length, distance };
		
	}

	return joints;

};


MStatus SplineIKChainControl::getSolution(const MObject& splineShape, const std::vector<IKControlSpec>& joints, std::vector<SplineIKSolution>& solution)
/**
Returns the default solution for the passed spline using the supplied joints.

@param splineShape: The curve to sample from.
@param joints: The joints to use when determining the distance along the curve.
@param solution: The passed array to populate.
@return: Return status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnNurbsCurve(splineShape, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Iterate through joints
	//
	size_t numJoints = joints.size();
	solution.resize(numJoints);

	double param, distance;
	MPoint point;

	for (size_t i = 0; i < numJoints; i++)
	{

		distance = (i > 0) ? joints[i].distance : 1e-3;

		param = fnNurbsCurve.findParamFromLength(distance, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = fnNurbsCurve.getPointAtParam(param, point);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		solution[i] = SplineIKSolution{ param, distance, joints[i].length, point };

	}

	return MS::kSuccess;

};


MStatus SplineIKChainControl::solve(const MObject& splineShape, const MVector& upVector, const MAngle& startTwistAngle, const MAngle& endTwistAngle, const std::vector<IKControlSpec>& joints, MMatrixArray& matrices)
/**
Returns a multi-pass solution for the supplied joint chain.
The first pass maps the bone length along the curve.
The second pass attempts to refine the position by incrementing along the curve using the excess bone length.
The solution uses the default forward-x and up-y axes!

@param splineShape: The curve to sample from.
@param upVector: The up-vector used to orient the joint chain.
@param startTwistAngle: The twist value at the start of the curve.
@param endTwistAngle: The twist value at the end of the curve.
@param joints: The joints in their respective parent spaces.
@param matrices: The passed array to populate with world-matrices.
@return: Return status.
*/
{

	MStatus status;

	// Check if there are enough joints
	//
	unsigned int numJoints = static_cast<unsigned int>(joints.size());

	if (numJoints < 2)
	{

		return MS::kFailure;

	}

	// Get base solution from spline and refine it
	//
	std::vector<SplineIKSolution> solution = std::vector<SplineIKSolution>();

	status = SplineIKChainControl::getSolution(splineShape, joints, solution);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = SplineIKChainControl::refineSolution(splineShape, this->iterationLimit, this->toleranceValue, solution);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Build matrix array from points and up-vector
	//
	MPointArray points;

	status = SplineIKChainControl::getSolutionPoints(solution, this->toleranceValue, points);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::createAimMatrix(points, 0, false, upVector, 1, false, matrices);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::twistMatrices(matrices, 0, startTwistAngle, endTwistAngle);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;

};


MStatus SplineIKChainControl::refineSolution(const MObject& splineShape, const unsigned int iterationLimit, const double tolerance, std::vector<SplineIKSolution> solution)
/**
Finds the next look-at solution for the supplied curve and point of origin.

@param splineShape: The curve to sample from.
@param iterationLimit: The maximum number of iterations per solution.
@param tolerance: The maximum offset allowed for a potential solution.
@param solution: The solution to refine.
@return: Return status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnNurbsCurve(splineShape, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	double maxDistance = fnNurbsCurve.length(1e-3, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Iterate through solutions
	// 
	size_t solutionCount = solution.size();
	
	MPoint origin, point;
	double param, distance, boneLength, difference;
	bool isClose;

	for (size_t i = 1; i < solutionCount; i++)
	{

		// Begin iterating through potential solutions
		// The iteration limit will keep us from looping infinitely
		//
		origin = MPoint(solution[i - 1].point);
		distance = double(solution[i].distance);
		boneLength = double(solution[i].boneLength);

		for (int j = 0; i < iterationLimit; i++)
		{

			// Get next point on curve
			//
			param = fnNurbsCurve.findParamFromLength(distance, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = fnNurbsCurve.getPointAtParam(param, point);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Check if traversed length is out of range
			// If so, then project the point off the end of the curve
			//
			if (distance >= maxDistance)
			{

				return MS::kSuccess;

			}

			// Evaluate distance to point in relation to bone length
			//
			distance = origin.distanceTo(point);
			difference = abs(distance - boneLength);

			isClose = (difference <= tolerance);

			if (isClose)
			{

				solution[i].param = param;
				solution[i].distance = distance;
				solution[i].point = point;
				break;

			}

			// Add or subtract difference from length and try again
			//
			if (distance > boneLength)
			{

				distance -= difference;

			}
			else
			{

				distance += difference;

			}

		}

	}

	return MS::kSuccess;

};


MStatus SplineIKChainControl::getSolutionPoints(const std::vector<SplineIKSolution>& solution, const double tolerance, MPointArray& points)
/**
Returns the points from the supplied solution while accomodating for bone lengths.

@param solution: The solution to sample from.
@param tolerance: The maximum offset allowed between points.
@param points: The passed array to populate.
@return: Return status.
*/
{

	MStatus status;

	// Check if there are enough solutions for points
	//
	unsigned int numPoints = static_cast<unsigned int>(solution.size());

	if (numPoints < 2)
	{

		return MS::kFailure;

	}

	// Allocate space for points
	//
	status = points.setLength(numPoints);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	points[0] = solution[0].point;  // Copy origin

	// Iterate through solutions
	//
	MVector aimVector, forwardVector;
	MPoint adjustedPoint;
	double distance, length;
	bool isClose, isZero;

	for (unsigned int i = 1; i < numPoints; i++)
	{

		// Check if aim vector is valid
		//
		aimVector = solution[i].point - solution[i - 1].point;
		distance = aimVector.length();
		length = solution[i].boneLength;

		isClose = Maxformations::isClose(distance, length, tolerance);

		if (isClose)
		{

			// No need to adjust any points
			//
			points[i] = solution[i].point;

		}
		else
		{

			// Check if distance is zero
			//
			isZero = Maxformations::isClose(0.0, distance, tolerance);

			if (!isZero)
			{

				forwardVector = (solution[i].point - points[i - 1]).normal();
				adjustedPoint = points[i - 1] + (forwardVector * length);

				points[i] = adjustedPoint;  // Adjust aim vector by applying bone length

			}
			else if (i >= 2)
			{

				forwardVector = (points[i - 1] - points[i - 2]).normal();
				adjustedPoint = points[i - 1] + (forwardVector * length);

				points[i] = adjustedPoint;  // Reuse previous solution

			}
			else
			{

				return MS::kFailure;

			}

		}

	}

	return MS::kSuccess;

};


bool SplineIKChainControl::setInternalValue(const MPlug& plug, const MDataHandle& handle)
/**
This method is overridden by nodes that store attribute data in some internal format.
The internal state of attributes can be set or queried using the setInternal and internal methods of MFnAttribute.
When internal attribute values are set via setAttr or MPlug::setValue this method is called.
Another use for this method is to impose attribute limits.

@param plug: The attribute that is being set.
@param handle: The dataHandle containing the value to set.
@return:
*/
{

	MStatus status;

	// Inspect plug
	//
	if (plug == SplineIKChainControl::tolerance)
	{

		this->toleranceValue = handle.asFloat();

	}
	else if (plug == SplineIKChainControl::iterations)
	{

		this->iterationLimit = handle.asInt();

	}
	else;

	return Matrix3Controller::setInternalValue(plug, handle);

};


MStatus SplineIKChainControl::legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal)
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
	if (plug == SplineIKChainControl::ikGoal && asSrc)
	{

		// Evaluate if other node is a `prs` node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = fnDependNode.typeId() == PRS::id;

		return MS::kSuccess;

	}
	else if ((plug == SplineIKChainControl::jointMatrix || (plug == SplineIKChainControl::jointPreferredRotationX || plug == SplineIKChainControl::jointPreferredRotationY || plug == SplineIKChainControl::jointPreferredRotationZ)) && asSrc)
	{

		// Evaluate if other node is a `ikControl` node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = fnDependNode.typeId() == IKControl::id;

		return MS::kSuccess;

	}
	else
	{

		return MPxNode::legalConnection(plug, otherPlug, asSrc, isLegal);

	}

};


MStatus SplineIKChainControl::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == SplineIKChainControl::ikGoal && !asSrc) && otherPlug == PRS::value)
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


MStatus SplineIKChainControl::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	if ((plug == SplineIKChainControl::ikGoal && !asSrc) && otherPlug == PRS::value)
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


bool SplineIKChainControl::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return false;

};


void* SplineIKChainControl::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ExposeTransform
*/
{

	return new SplineIKChainControl();

};


MStatus SplineIKChainControl::initialize()
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
	SplineIKChainControl::enabled = fnNumericAttr.create("enabled", "e", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".ikGoal" attribute
	//
	SplineIKChainControl::ikGoal = fnTypedAttr.create("ikGoal", "ikg", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".ikParentMatrix" attribute
	//
	SplineIKChainControl::ikParentMatrix = fnTypedAttr.create("ikParentMatrix", "ipm", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".forwardAxis" attribute
	//
	SplineIKChainControl::forwardAxis = fnEnumAttr.create("forwardAxis", "fa", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField(MString("x"), 0));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("y"), 1));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("z"), 2));

	// ".forwardAxisFlip" attribute
	//
	SplineIKChainControl::forwardAxisFlip = fnNumericAttr.create("forwardAxisFlip", "faf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".upAxis" attribute
	//
	SplineIKChainControl::upAxis = fnEnumAttr.create("upAxis", "ua", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField(MString("x"), 0));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("y"), 1));
	CHECK_MSTATUS(fnEnumAttr.addField(MString("z"), 2));

	// ".upAxisFlip" attribute
	//
	SplineIKChainControl::upAxisFlip = fnNumericAttr.create("upAxisFlip", "uaf", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotationX" attribute
	//
	SplineIKChainControl::jointPreferredRotationX = fnUnitAttr.create("jointPreferredRotationX", "jprx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotationY" attribute
	//
	SplineIKChainControl::jointPreferredRotationY = fnUnitAttr.create("jointPreferredRotationY", "jpry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotationZ" attribute
	//
	SplineIKChainControl::jointPreferredRotationZ = fnUnitAttr.create("jointPreferredRotationZ", "jprz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointPreferredRotation" attribute
	//
	SplineIKChainControl::jointPreferredRotation = fnNumericAttr.create("jointPreferredRotation", "jpr", SplineIKChainControl::jointPreferredRotationX, SplineIKChainControl::jointPreferredRotationY, SplineIKChainControl::jointPreferredRotationZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotationX" attribute
	//
	SplineIKChainControl::jointOffsetRotationX = fnUnitAttr.create("jointOffsetRotationX", "jorx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotationY" attribute
	//
	SplineIKChainControl::jointOffsetRotationY = fnUnitAttr.create("jointOffsetRotationY", "jory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotationZ" attribute
	//
	SplineIKChainControl::jointOffsetRotationZ = fnUnitAttr.create("jointOffsetRotationZ", "jorz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointOffsetRotation" attribute
	//
	SplineIKChainControl::jointOffsetRotation = fnNumericAttr.create("jointOffsetRotation", "jor", SplineIKChainControl::jointOffsetRotationX, SplineIKChainControl::jointOffsetRotationY, SplineIKChainControl::jointOffsetRotationZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".jointMatrix" attribute
	//
	SplineIKChainControl::jointMatrix = fnTypedAttr.create("jointMatrix", "jm", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".joint" attribute
	//
	SplineIKChainControl::joint = fnCompoundAttr.create("joint", "jnt", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(SplineIKChainControl::jointPreferredRotation));
	CHECK_MSTATUS(fnCompoundAttr.addChild(SplineIKChainControl::jointOffsetRotation));
	CHECK_MSTATUS(fnCompoundAttr.addChild(SplineIKChainControl::jointMatrix));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	
	// ".jointParentMatrix" attribute
	//
	SplineIKChainControl::jointParentMatrix = fnTypedAttr.create("jointParentMatrix", "jpm", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".splineShape" attribute
	//
	SplineIKChainControl::splineShape = fnTypedAttr.create("splineShape", "ss", MFnData::kNurbsCurve, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".startTwistAngle" attribute
	//
	SplineIKChainControl::startTwistAngle = fnUnitAttr.create("startTwistAngle", "sta", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".endTwistAngle" attribute
	//
	SplineIKChainControl::endTwistAngle = fnUnitAttr.create("endTwistAngle", "eta", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".useUpNode" attribute
	//
	SplineIKChainControl::useUpNode = fnNumericAttr.create("useUpNode", "uun", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".upNode" attribute
	//
	SplineIKChainControl::upNode = fnTypedAttr.create("upNode", "un", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// ".iterations" attribute
	//
	SplineIKChainControl::iterations = fnNumericAttr.create("iterations", "i", MFnNumericData::kInt, 10, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(1));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));

	// ".iterations" attribute
	//
	SplineIKChainControl::tolerance = fnNumericAttr.create("tolerance", "t", MFnNumericData::kFloat, 1e-3, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));

	// Output attributes:
	// ".goal" attribute
	//
	SplineIKChainControl::goal = fnTypedAttr.create("goal", "g", MFnData::kMatrix, Matrix3Controller::IDENTITY_MATRIX_DATA, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.setWritable(false));
	CHECK_MSTATUS(fnTypedAttr.setStorable(false));
	CHECK_MSTATUS(fnTypedAttr.setArray(true));
	CHECK_MSTATUS(fnTypedAttr.setUsesArrayDataBuilder(true));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(SplineIKChainControl::goalCategory));

	// Inherit attributes from parent class
	//
	status = SplineIKChainControl::inheritAttributesFrom("matrix3Controller");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Add attributes to node
	//
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::enabled));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::ikGoal));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::ikParentMatrix));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::forwardAxis));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::forwardAxisFlip));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::upAxis));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::upAxisFlip));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::joint));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::jointParentMatrix));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::splineShape));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::startTwistAngle));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::endTwistAngle));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::useUpNode));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::upNode));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::iterations));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::tolerance));
	CHECK_MSTATUS(SplineIKChainControl::addAttribute(SplineIKChainControl::goal));

	// Define attribute relationships
	//
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::enabled, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::ikGoal, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::ikParentMatrix, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::forwardAxis, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::forwardAxisFlip, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::upAxis, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::upAxisFlip, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::jointPreferredRotation, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::jointOffsetRotation, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::jointMatrix, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::jointParentMatrix, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::splineShape, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::startTwistAngle, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::endTwistAngle, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::useUpNode, SplineIKChainControl::goal));
	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::upNode, SplineIKChainControl::goal));

	CHECK_MSTATUS(SplineIKChainControl::attributeAffects(SplineIKChainControl::ikGoal, SplineIKChainControl::value));

	return status;

};