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

MObject	SplineIKChainControl::goal;

MString	SplineIKChainControl::inputCategory("Input");
MString	SplineIKChainControl::goalCategory("Goal");

MTypeId	SplineIKChainControl::id(0x0013b1d3);


SplineIKChainControl::SplineIKChainControl() : Matrix3Controller() {};
SplineIKChainControl::~SplineIKChainControl() {};


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
	double length = 0.0;

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

		joints[i] = IKControlSpec{ preferredRotation, offsetRotation, matrix, length };
		
	}

	return joints;

};


double SplineIKChainControl::getChainLength(const std::vector<IKControlSpec>& joints)
/**
Returns the length of the supplied joint chain.

@param joints: The joint chain to add up.
@return: Chain length.
*/
{

	size_t numJoints = joints.size();
	double length = 0.0;

	for (size_t i = 0; i < numJoints; i++)
	{

		length += joints[i].length;

	}

	return length;

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

	if (!(numJoints > 2))
	{

		return MS::kFailure;

	}

	// Get spline samples
	//
	double chainLength = SplineIKChainControl::getChainLength(joints);

	MPointArray samples;

	status = SplineIKChainControl::getSplineSamples(splineShape, numJoints, chainLength, samples);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Build matrix array from points and up-vector
	//
	MPointArray solution;

	status = SplineIKChainControl::findSolution(samples, joints, solution);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::createAimMatrix(solution, 0, false, upVector, 1, false, matrices);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::twistMatrices(matrices, 0, startTwistAngle, endTwistAngle);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;

};


MStatus SplineIKChainControl::getSplineSamples(const MObject& splineShape, unsigned int numJoints, const double chainLength, MPointArray& samples)
/**
Decomposes the supplied spline shape into a series of data points.
The number of samples is equivalent to: 2 * (2 ^ numJoints).
The first half of samples consists of points along the curve and the second half are reserved for projecting the curve's end tangent.

@param splineShape: The curve to sample from.
@param numJoints: The number of joints in the chain.
@param chainLength: The length of the joint chain.
@param samples: The passed array to populate.
@return: Return status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnNurbsCurve(splineShape, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Resize sample array
	//
	unsigned int sampleCount = pow(numJoints, 2);

	status = samples.setLength(sampleCount + 1);  // Reserve a sample for the end tangent!
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Iterate through joints
	//
	double splineLength = fnNurbsCurve.length(1e-3, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	double percentage = splineLength / static_cast<double>(sampleCount - 1);
	double distance, param;

	for (unsigned int i = 0; i < sampleCount; i++)
	{

		distance = percentage * static_cast<double>(i);

		param = fnNurbsCurve.findParamFromLength(distance, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = fnNurbsCurve.getPointAtParam(param, samples[i]);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	// Extend samples using curve's end tangent
	//
	param = fnNurbsCurve.findParamFromLength(splineLength, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MVector tangent = fnNurbsCurve.tangent(param, MSpace::kObject, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	samples[sampleCount] = samples[sampleCount - 1] + (tangent.normal() * chainLength);

	return MS::kSuccess;

};


MStatus SplineIKChainControl::findSolution(const MPointArray& points, const std::vector<IKControlSpec>& joints, MPointArray& solutions)
/**
Finds a solution that fits supplied joint chain to the sample points.

@param points: The sample points to align the joints to.
@param joints: The joints to find solutions for.
@param solutions: The passed array to populate.
@return: Return status.
*/
{

	// Resize passed array
	//
	unsigned int numJoints = static_cast<unsigned int>(joints.size());

	MStatus status = solutions.setLength(numJoints);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	solutions[0] = points[0];

	// Iterate through joints
	//
	unsigned int numPoints = points.length();
	unsigned int lastSegment = numPoints - 1;
	unsigned int segment = 0;

	MPoint origin, startPoint, endPoint;
	double startDistance, endDistance, boneLength;
	MPointArray hits;
	unsigned hitCount;

	for (unsigned int i = 1; i < numJoints; i++)
	{

		// Iterate through point samples
		//
		boneLength = joints[i].length;
		origin = solutions[i - 1];

		for (unsigned int j = segment; j < lastSegment; j++)
		{

			// Check if bone is in range
			//
			startPoint = points[j];
			startDistance = origin.distanceTo(startPoint);

			endPoint = points[j + 1];
			endDistance = origin.distanceTo(endPoint);
			
			if (!(boneLength <= endDistance))
			{

				continue;

			}

			// Calculate point in segment
			//
			hits = SplineIKChainControl::lineSphereIntersection(startPoint, endPoint, origin, boneLength);
			hitCount = hits.length();

			//debugIntersection(i, startPoint, endPoint, origin, boneLength, hits);

			if (hitCount > 0)
			{

				solutions[i] = hits[hitCount - 1];
				segment = ((j + 1) < lastSegment) ? (j + 1) : segment;  // Move starting segment in point array

				break;

			}
			else
			{
				
				continue;

			}

		}

	}

	return MS::kSuccess;

};


void SplineIKChainControl::debugIntersection(const unsigned int index, const MPoint& startPoint, const MPoint& endPoint, const MPoint& center, const double radius, const MPointArray& hits)
/**
Prints the supplied intersection parameters to the output window.

@param index: Index of the joint.
@param startPoint: The start of the line.
@param endPoint: The end of the line.
@param center: The center of the sphere.
@param radius: The radius of the sphere.
@param hits: The found hits.
@return: void.
*/
{

	MString debug;

	debug += "joint[";
	debug += index;
	debug += "]\n";

	debug += "Start Point = <<";
	debug += startPoint.x;
	debug += ", ";
	debug += startPoint.y;
	debug += ", ";
	debug += startPoint.z;
	debug += ">>\n";

	debug += "End Point = <<";
	debug += endPoint.x;
	debug += ", ";
	debug += endPoint.y;
	debug += ", ";
	debug += endPoint.z;
	debug += ">>\n";

	debug += "Center = <<";
	debug += center.x;
	debug += ", ";
	debug += center.y;
	debug += ", ";
	debug += center.z;
	debug += ">>\n";

	debug += "Radius = ";
	debug += radius;
	debug += "\n";

	for (unsigned int i = 0; i < hits.length(); i++)
	{

		debug += "Hit = <<";
		debug += hits[i].x;
		debug += ", ";
		debug += hits[i].y;
		debug += ", ";
		debug += hits[i].z;
		debug += ">>\n";

	}

	MGlobal::displayInfo(debug);

};


MPointArray SplineIKChainControl::lineSphereIntersection(const MPoint& startPoint, const MPoint& endPoint, const MPoint& center, const double radius)
/**
Returns the collision points between the supplied line and sphere.
See the following for details: https://stackoverflow.com/questions/6533856/ray-sphere-intersection

@param startPoint: The start of the line.
@param endPoint: The end of the line.
@param center: The center of the sphere.
@param radius: The radius of the sphere.
@return: The points of collision.
*/
{

	// Calculate the discriminant
	//
	double ax = startPoint.x, ay = startPoint.y, az = startPoint.z;
	double bx = endPoint.x, by = endPoint.y, bz = endPoint.z;
	double cx = center.x, cy = center.y, cz = center.z;

	double a = pow(bx - ax, 2.0) + pow(by - ay, 2.0) + pow(bz - az, 2.0);
	double b = 2.0 * ((bx - ax) * (ax - cx) + (by - ay) * (ay - cy) + (bz - az) * (az - cz));
	double c = pow(ax - cx, 2.0) + pow(ay - cy, 2.0) + pow(az - cz, 2.0) - pow(radius, 2.0);

	double discriminant = pow(b, 2.0) - 4.0 * a * c;

	if (discriminant < 0.0)
	{

		return MPointArray();

	}

	// Evaluate discriminant
	// This determines how many points of contact there are
	//
	double t1 = (-b - sqrt(discriminant)) / (2.0 * a);
	double t2 = (-b + sqrt(discriminant)) / (2.0 * a);

	MPoint solution1 = MPoint(Maxformations::lerp(ax, bx, t1), Maxformations::lerp(ay, by, t1), Maxformations::lerp(az, bz, t1));
	MPoint solution2 = MPoint(Maxformations::lerp(ax, bx, t2), Maxformations::lerp(ay, by, t2), Maxformations::lerp(az, bz, t2));

	MPointArray points;

	if ((0.0 <= t1 && t1 <= 1.0) && (0.0 <= t2 && t2 <= 1.0))  // 2 solutions
	{

		points.setLength(2);
		points[0] = solution1;
		points[1] = solution2;

	}
	else if (0.0 <= t1 && t1 <= 1.0)  // 1 solution
	{

		points.setLength(1);
		points[0] = solution1;

	}
	else if (0.0 <= t2 && t2 <= 1.0)  // 1 solution
	{

		points.setLength(1);
		points[0] = solution2;

	}
	else;

	return points;

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

		// Evaluate if other node is supported
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isPRS = Maxformations::hasTypeId(otherNode, PRS::id, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		isLegal = isPRS;

		return MS::kSuccess;

	}
	else if ((plug == SplineIKChainControl::jointMatrix || (plug == SplineIKChainControl::jointPreferredRotationX || plug == SplineIKChainControl::jointPreferredRotationY || plug == SplineIKChainControl::jointPreferredRotationZ)) && asSrc)
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