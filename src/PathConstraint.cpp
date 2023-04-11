//
// File: PathConstraintNode.cpp
//
// Dependency Graph Node: PathConstraint
//
// Author: Benjamin H. Singleton
//

#include "PathConstraint.h"


MObject PathConstraint::percent;
MObject PathConstraint::useParameter;
MObject PathConstraint::loop;
MObject PathConstraint::forwardAxis;
MObject PathConstraint::forwardAxisFlip;
MObject PathConstraint::twist;
MObject PathConstraint::upAxis;
MObject PathConstraint::upAxisFlip;
MObject PathConstraint::worldUpType;
MObject PathConstraint::worldUpVector;
MObject PathConstraint::worldUpVectorX;
MObject PathConstraint::worldUpVectorY;
MObject PathConstraint::worldUpVectorZ;
MObject PathConstraint::worldUpMatrix;
MObject PathConstraint::relative;
MObject PathConstraint::offsetTranslate;
MObject PathConstraint::offsetTranslateX;
MObject PathConstraint::offsetTranslateY;
MObject PathConstraint::offsetTranslateZ;
MObject PathConstraint::offsetRotate;
MObject PathConstraint::offsetRotateX;
MObject PathConstraint::offsetRotateY;
MObject PathConstraint::offsetRotateZ;
MObject PathConstraint::restTranslate;
MObject PathConstraint::restTranslateX;
MObject PathConstraint::restTranslateY;
MObject PathConstraint::restTranslateZ;
MObject PathConstraint::restRotate;
MObject PathConstraint::restRotateX;
MObject PathConstraint::restRotateY;
MObject PathConstraint::restRotateZ;

MObject PathConstraint::target;
MObject PathConstraint::targetWeight;
MObject PathConstraint::targetCurve;

MObject PathConstraint::constraintTranslate;
MObject PathConstraint::constraintTranslateX;
MObject PathConstraint::constraintTranslateY;
MObject PathConstraint::constraintTranslateZ;
MObject PathConstraint::constraintRotate;
MObject PathConstraint::constraintRotateX;
MObject PathConstraint::constraintRotateY;
MObject PathConstraint::constraintRotateZ;
MObject PathConstraint::constraintRotateOrder;
MObject PathConstraint::constraintMatrix;
MObject PathConstraint::constraintInverseMatrix;
MObject PathConstraint::constraintWorldMatrix;
MObject PathConstraint::constraintWorldInverseMatrix;
MObject PathConstraint::constraintParentInverseMatrix;

MString	PathConstraint::inputCategory("Input");
MString	PathConstraint::offsetCategory("Offset");
MString	PathConstraint::restCategory("Rest");
MString	PathConstraint::targetCategory("Target");
MString	PathConstraint::outputCategory("Output");

MTypeId PathConstraint::id(0x0013b1c3);


PathConstraint::PathConstraint() {}
PathConstraint::~PathConstraint() {};


MStatus PathConstraint::compute(const MPlug& plug, MDataBlock& data)
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

	if (fnAttribute.hasCategory(PathConstraint::outputCategory))
	{

		// Get input data handles
		//
		MDataHandle percentHandle = data.inputValue(PathConstraint::percent, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle useParameterHandle = data.inputValue(PathConstraint::useParameter, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle loopHandle = data.inputValue(PathConstraint::loop, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle forwardAxisHandle = data.inputValue(PathConstraint::forwardAxis, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle forwardAxisFlipHandle = data.inputValue(PathConstraint::forwardAxisFlip, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle twistHandle = data.inputValue(PathConstraint::twist, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle upAxisHandle = data.inputValue(PathConstraint::upAxis, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle upAxisFlipHandle = data.inputValue(PathConstraint::upAxisFlip, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldUpTypeHandle = data.inputValue(PathConstraint::worldUpType, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldUpVectorHandle = data.inputValue(PathConstraint::worldUpVector, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle worldUpMatrixHandle = data.inputValue(PathConstraint::worldUpMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle relativeHandle = data.inputValue(PathConstraint::relative, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle offsetTranslateHandle = data.inputValue(PathConstraint::offsetTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle offsetRotateHandle = data.inputValue(PathConstraint::offsetRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle enableRestPositionHandle = data.inputValue(PathConstraint::enableRestPosition, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle restTranslateHandle = data.inputValue(PathConstraint::restTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle restRotateHandle = data.inputValue(PathConstraint::restRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateOrderHandle = data.inputValue(PathConstraint::constraintRotateOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintParentInverseMatrixHandle = data.inputValue(PathConstraint::constraintParentInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle targetArrayHandle = data.inputArrayValue(PathConstraint::target, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		double percent = percentHandle.asDouble();
		bool useParameter = useParameterHandle.asBool();
		bool loop = loopHandle.asBool();
		int forwardAxis = forwardAxisHandle.asShort();
		bool forwardAxisFlip = forwardAxisFlipHandle.asBool();
		MAngle twistAngle = twistHandle.asAngle();
		int upAxis = upAxisHandle.asShort();
		bool upAxisFlip = upAxisFlipHandle.asBool();
		AxisSettings axisSettings = { forwardAxis, forwardAxisFlip, upAxis, upAxisFlip };
		short worldUpType = worldUpTypeHandle.asShort();
		MVector worldUpVector = worldUpVectorHandle.asVector();
		MMatrix worldUpMatrix = worldUpMatrixHandle.asMatrix();
		WorldUpSettings worldUpSettings = { worldUpType, worldUpVector, worldUpMatrix };

		MEulerRotation::RotationOrder constraintRotateOrder = MEulerRotation::RotationOrder(constraintRotateOrderHandle.asShort());
		MMatrix constraintParentInverseMatrix = constraintParentInverseMatrixHandle.asMatrix();
		MMatrix constraintParentMatrix = constraintParentInverseMatrix.inverse();

		bool relative = relativeHandle.asBool();
		MVector offsetTranslate = offsetTranslateHandle.asVector();
		MMatrix offsetTranslateMatrix = Maxformations::createPositionMatrix(offsetTranslate);
		MVector offsetRotate = offsetRotateHandle.asVector();
		MMatrix offsetRotateMatrix = Maxformations::createRotationMatrix(offsetRotate);
		MMatrix offsetMatrix = relative ? (offsetRotateMatrix * offsetTranslateMatrix) : MMatrix::identity;

		bool restEnabled = enableRestPositionHandle.asBool();
		MVector restTranslate = restTranslateHandle.asVector();
		MMatrix restTranslateMatrix = Maxformations::createPositionMatrix(restTranslate);
		MVector restRotate = restRotateHandle.asVector();
		MMatrix restRotateMatrix = Maxformations::createRotationMatrix(restRotate, constraintRotateOrder);
		MMatrix restMatrix = restEnabled ? (restRotateMatrix * restTranslateMatrix) : MMatrix::identity;
		MMatrix restWorldMatrix = restMatrix * constraintParentMatrix;

		// Create forward twist matrix
		//
		MMatrix twistMatrix;

		status = Maxformations::createTwistMatrix(forwardAxis, twistAngle, twistMatrix);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Initialize target matrices
		//
		unsigned int targetCount = targetArrayHandle.elementCount();

		MFloatArray targetWeights = MFloatArray(targetCount);
		MMatrixArray targetMatrices = MMatrixArray(targetCount);

		// Iterate through targets
		//
		MDataHandle targetHandle, targetWeightHandle, targetCurveHandle;

		MObject targetCurve;
		double parameter, maxParameter, percentage;
		MPoint position;
		MVector forwardVector, upVector;
		MMatrix targetMatrix;

		for (unsigned int i = 0; i < targetCount; i++)
		{

			// Jump to array element
			//
			status = targetArrayHandle.jumpToElement(i);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			targetHandle = targetArrayHandle.inputValue(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get target data handles
			//
			targetWeightHandle = targetHandle.child(PathConstraint::targetWeight);
			targetCurveHandle = targetHandle.child(PathConstraint::targetCurve);

			// Get weight value
			//
			targetWeights[i] = targetWeightHandle.asFloat() / 100.0;

			// Get curve parameter
			//
			targetCurve = targetCurveHandle.asNurbsCurve();

			if (useParameter)
			{

				status = PathConstraint::getCurveParamLength(targetCurve, maxParameter);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				parameter = loop ? Maxformations::loop(percent, 0.0, maxParameter) : Maxformations::clamp(percent, 0.0, maxParameter);

			}
			else
			{

				percentage = loop ? Maxformations::loop(percent, 0.0, 100.0) : Maxformations::clamp(percent, 0.0, 100.0);

				status = PathConstraint::getParamFromPercentage(targetCurve, percentage, parameter);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

			// Sample curve at parameter
			//
			status = PathConstraint::sampleCurveAtParameter(targetCurve, parameter, axisSettings, worldUpSettings, position, forwardVector, upVector);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Compose transform matrix
			//
			status = Maxformations::createAimMatrix(forwardVector, forwardAxis, upVector, upAxis, position, targetMatrix);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			targetMatrices[i] = twistMatrix * targetMatrix;

		}

		// Calculate weighted constraint matrix
		//
		MMatrix pathMatrix = Maxformations::blendMatrices(restWorldMatrix, targetMatrices, targetWeights);

		MMatrix constraintWorldMatrix = offsetMatrix * pathMatrix;
		MMatrix constraintMatrix = constraintWorldMatrix * constraintParentInverseMatrix;

		// Get output data handles
		//
		MDataHandle constraintTranslateXHandle = data.outputValue(PathConstraint::constraintTranslateX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintTranslateYHandle = data.outputValue(PathConstraint::constraintTranslateY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintTranslateZHandle = data.outputValue(PathConstraint::constraintTranslateZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateXHandle = data.outputValue(PathConstraint::constraintRotateX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateYHandle = data.outputValue(PathConstraint::constraintRotateY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateZHandle = data.outputValue(PathConstraint::constraintRotateZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintMatrixHandle = data.outputValue(PathConstraint::constraintMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintInverseMatrixHandle = data.outputValue(PathConstraint::constraintInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldMatrixHandle = data.outputValue(PathConstraint::constraintWorldMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldInverseMatrixHandle = data.outputValue(PathConstraint::constraintWorldInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update constraint translation
		//
		MVector constraintTranslate = Maxformations::matrixToPosition(constraintMatrix);
		MDistance::Unit distanceUnit = MDistance::internalUnit();

		constraintTranslateXHandle.setMDistance(MDistance(constraintTranslate.x, distanceUnit));
		constraintTranslateYHandle.setMDistance(MDistance(constraintTranslate.y, distanceUnit));
		constraintTranslateZHandle.setMDistance(MDistance(constraintTranslate.z, distanceUnit));

		constraintTranslateXHandle.setClean();
		constraintTranslateYHandle.setClean();
		constraintTranslateZHandle.setClean();

		// Update constraint rotation
		//
		MEulerRotation constraintRotate = Maxformations::matrixToEulerRotation(constraintMatrix, constraintRotateOrder);
		MAngle::Unit angleUnit = MAngle::internalUnit();

		constraintRotateXHandle.setMAngle(MAngle(constraintRotate.x, angleUnit));
		constraintRotateYHandle.setMAngle(MAngle(constraintRotate.y, angleUnit));
		constraintRotateZHandle.setMAngle(MAngle(constraintRotate.z, angleUnit));

		constraintRotateXHandle.setClean();
		constraintRotateYHandle.setClean();
		constraintRotateZHandle.setClean();

		// Update constraint matrices
		//
		constraintMatrixHandle.setMMatrix(constraintMatrix);
		constraintInverseMatrixHandle.setMMatrix(constraintMatrix.inverse());
		constraintWorldMatrixHandle.setMMatrix(constraintWorldMatrix);
		constraintWorldInverseMatrixHandle.setMMatrix(constraintWorldMatrix.inverse());

		constraintMatrixHandle.setClean();
		constraintInverseMatrixHandle.setClean();
		constraintWorldMatrixHandle.setClean();
		constraintWorldInverseMatrixHandle.setClean();

		// Mark data block as clean
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


MStatus	PathConstraint::sampleCurveAtParameter(const MObject& curve, const double parameter, const AxisSettings& axisSettings, const WorldUpSettings& worldUpSettings, MPoint& position, MVector& forwardVector, MVector& upVector)
/**
Samples the supplied curve at the specified parameter.

@param curve: The curve data object to sample from.
@param parameter: The curve parameter to sample at.
@param point: The position at the specified parameter.
@param forwardVector: The forward-vector at the specified parameter.
@param upVector: The up-vector at the specified parameter.
@return: Return status.
*/
{

	MStatus status;

	// Get curve point
	//
	status = PathConstraint::getCurvePoint(curve, parameter, position);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Get forward-vector
	//
	status = PathConstraint::getForwardVector(curve, parameter, forwardVector);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (axisSettings.forwardAxisFlip)
	{

		forwardVector *= -1.0;

	}

	// Get up-vector
	//
	status = PathConstraint::getUpVector(curve, parameter, worldUpSettings, position, upVector);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (axisSettings.upAxisFlip)
	{

		upVector *= -1.0;

	}

	return status;

};


MStatus PathConstraint::getForwardVector(const MObject& curve, const double parameter, MVector& forwardVector)
/**
Returns the forward vector at the specified parameter.

@param curve: The curve data object to sample from.
@param parameter: The curve parameter to sample at.
@param forwardVector: The passed vector to populate.
@return: Return status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnCurve(curve, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Return tagent vector at parameter
	//
	forwardVector = fnCurve.tangent(parameter, MSpace::kWorld, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = forwardVector.normalize();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;

};


MStatus PathConstraint::getUpVector(const MObject& curve, const double parameter, const WorldUpSettings& settings, const MVector& position, MVector& upVector)
/**
Returns the up vector based on the selected world up type.

@param data: Data block containing storage for the node's attributes.
@param parameter: The curve parameter to sample at.
@param curve: The curve data object to sample from.
@param upVector: The passed vector to populate.
@return: Return status.
*/
{

	MStatus status;

	// Check world up type
	//
	switch (settings.worldUpType)
	{

	case 0: // Scene Up
	{

		upVector = MGlobal::upAxis();

	}
	break;

	case 1: // Object Up
	{

		upVector = (MVector(position) - MVector(settings.worldUpMatrix[3])).normal();

	}
	break;

	case 2: // Object Rotation Up
	{

		upVector = PathConstraint::getObjectRotationUpVector(settings.worldUpVector, settings.worldUpMatrix);

	}
	break;

	case 3: // Vector
	{

		upVector = MVector(settings.worldUpVector).normal();

	}
	break;

	case 4: // Curve normal
	{

		status = PathConstraint::getCurveNormal(curve, parameter, upVector);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}
	break;

	default:
	{

		return MS::kFailure;

	}
	break;

	}

	return MS::kSuccess;

};


MVector PathConstraint::getObjectRotationUpVector(const MVector& worldUpVector, const MMatrix& worldUpMatrix)
/**
Returns the weighted up-vector derived from the supplied world-matrix.

@param worldUpVector: The weighted values to average from.
@param worldUpMatrix: The world matrix of the up object.
@return: MVector
*/
{

	// Extract axis vectors
	//
	MVector xAxis = MVector(worldUpMatrix[0]);
	MVector yAxis = MVector(worldUpMatrix[1]);
	MVector zAxis = MVector(worldUpMatrix[2]);

	// Calculate weighted vector average
	//
	return MVector((xAxis * worldUpVector.x) + (yAxis * worldUpVector.y) + (zAxis * worldUpVector.z)).normal();

};


MStatus	PathConstraint::getCurvePoint(const MObject& curve, const double parameter, MPoint& position)
/**
Returns the point on a curve from the given percentile.

@param curve: The curve data object to sample from.
@param parameter: The curve parameter to sample at.
@param position: The passed point to populate.
@return: Return status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnCurve(curve, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Return normal at parameter
	//
	status = fnCurve.getPointAtParam(parameter, position, MSpace::kWorld);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus	PathConstraint::getCurveNormal(const MObject& curve, const double parameter, MVector& upVector)
/**
Returns the tangent normal at the given percentile.

@param curve: The curve data object to sample from.
@param parameter: The curve parameter to sample at.
@param upVector: The passed vector to populate.
@return: Return status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnCurve(curve, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Return normal at parameter
	//
	upVector = fnCurve.normal(parameter, MSpace::kWorld, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus PathConstraint::getParamFromPercentage(const MObject& curve, const double percentage, double& parameter)
/**
Returns the curve parameter based on the fractional distance across the curve.

@param curve: The curve data object to sample from.
@param fraction: The fractional distance, between 0-1, to sample from.
@param parameter: The passed parameter to populate.
@return: Return status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnCurve(curve, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Calculate the fractional distance
	//
	double fraction = Maxformations::clamp(percentage, 0.0, 100.0) / 100.0;

	double curveLength = fnCurve.length();
	double paramLength = fraction * curveLength;

	if (paramLength == 0.0)
	{

		paramLength += 0.001;

	}
	else if (paramLength == curveLength)
	{

		paramLength -= 0.001;

	}
	else;

	// Get param from length
	//
	parameter = fnCurve.findParamFromLength(paramLength, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus PathConstraint::getCurveParamLength(const MObject& curve, double& maxParameter)
{

	MStatus status;

	// Initialize function set
	//
	MFnNurbsCurve fnCurve(curve, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Find max parameter from curve length
	//
	double length = fnCurve.length(1e-3, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	maxParameter = fnCurve.findParamFromLength(length, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;

};


const MObject PathConstraint::targetAttribute() const
/**
Returns the target attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return PathConstraint::target;

};


const MObject PathConstraint::weightAttribute() const
/**
Returns the weight attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return PathConstraint::targetWeight;

};


const MObject PathConstraint::constraintRotateOrderAttribute() const
/**
Returns the rotate order attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return PathConstraint::constraintRotateOrder;

};


void* PathConstraint::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: PathConstraint
*/
{

	return new PathConstraint();

};


MStatus PathConstraint::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{

	MStatus	status;

	// Declare attribute function sets
	//
	MFnCompoundAttribute fnCompoundAttr;
	MFnNumericAttribute fnNumericAttr;
	MFnUnitAttribute fnUnitAttr;
	MFnTypedAttribute fnTypedAttr;
	MFnEnumAttribute fnEnumAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnMessageAttribute fnMessageAttr;

	// Input attributes:
	// ".percent" attribute
	//
	PathConstraint::percent = fnNumericAttr.create("percent", "pct", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));

	// ".useParameter" attribute
	//
	PathConstraint::useParameter = fnNumericAttr.create("useParameter", "up", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));

	// ".loop" attribute
	//
	PathConstraint::loop = fnNumericAttr.create("loop", "lp", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));

	// ".forwardAxis" attribute
	//
	PathConstraint::forwardAxis = fnEnumAttr.create("forwardAxis", "fa", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("X", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("Y", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("Z", 2));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PathConstraint::inputCategory));

	// ".forwardAxisFlip" attribute
	//
	PathConstraint::forwardAxisFlip = fnNumericAttr.create("forwardAxisFlip", "faf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));

	// ".twist" attribute
	//
	PathConstraint::twist = fnUnitAttr.create("twist", "twst", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));

	// ".upAxis" attribute
	//
	PathConstraint::upAxis = fnEnumAttr.create("upAxis", "ua", short(2), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("X", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("Y", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("Z", 2));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PathConstraint::inputCategory));

	// ".upAxisFlip" attribute
	//
	PathConstraint::upAxisFlip = fnNumericAttr.create("upAxisFlip", "uaf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));

	// ".worldUpType" attribute
	//
	PathConstraint::worldUpType = fnEnumAttr.create("worldUpType", "wut", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("Scene Up", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("Object Up", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("Object Rotation Up", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("Vector", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("Normal", 4));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PathConstraint::inputCategory));

	// ".worldUpVectorX" attribute
	//
	PathConstraint::worldUpVectorX = fnUnitAttr.create("worldUpVectorX", "wuvx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));

	// ".worldUpVectorY" attribute
	//
	PathConstraint::worldUpVectorY = fnUnitAttr.create("worldUpVectorY", "wuvy", MFnUnitAttribute::kDistance, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));

	// ".worldUpVectorZ" attribute
	//
	PathConstraint::worldUpVectorZ = fnUnitAttr.create("worldUpVectorZ", "wuvz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));

	// ".worldUpVector" attribute
	//
	PathConstraint::worldUpVector = fnNumericAttr.create("worldUpVector", "wuv", PathConstraint::worldUpVectorX, PathConstraint::worldUpVectorY, PathConstraint::worldUpVectorZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));

	// ".worldUpMatrix" attribute
	//
	PathConstraint::worldUpMatrix = fnMatrixAttr.create("worldUpMatrix", "wum", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PathConstraint::inputCategory));

	// ".relative" attribute
	//
	PathConstraint::relative = fnNumericAttr.create("relative", "rel", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));

	// ".offsetTranslateX" attribute
	//
	PathConstraint::offsetTranslateX = fnUnitAttr.create("offsetTranslateX", "otx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::offsetCategory));

	// ".offsetTranslateY" attribute
	//
	PathConstraint::offsetTranslateY = fnUnitAttr.create("offsetTranslateY", "oty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::offsetCategory));

	// ".offsetTranslateZ" attribute
	//
	PathConstraint::offsetTranslateZ = fnUnitAttr.create("offsetTranslateZ", "otz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::offsetCategory));

	// ".offsetTranslate" attribute
	//
	PathConstraint::offsetTranslate = fnNumericAttr.create("offsetTranslate", "ot", PathConstraint::offsetTranslateX, PathConstraint::offsetTranslateY, PathConstraint::offsetTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::offsetCategory));

	// ".offsetRotateX" attribute
	//
	PathConstraint::offsetRotateX = fnUnitAttr.create("offsetRotateX", "orx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::offsetCategory));

	// ".offsetRotateY" attribute
	//
	PathConstraint::offsetRotateY = fnUnitAttr.create("offsetRotateY", "ory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::offsetCategory));

	// ".offsetRotateZ" attribute
	//
	PathConstraint::offsetRotateZ = fnUnitAttr.create("offsetRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::offsetCategory));

	// ".offsetRotate" attribute
	//
	PathConstraint::offsetRotate = fnNumericAttr.create("offsetRotate", "or", PathConstraint::offsetRotateX, PathConstraint::offsetRotateY, PathConstraint::offsetRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::offsetCategory));

	// ".enableRestPosition" attribute
	//
	status = fnNumericAttr.setObject(PathConstraint::enableRestPosition);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::restCategory));

	// ".restTranslateX" attribute
	//
	PathConstraint::restTranslateX = fnUnitAttr.create("restTranslateX", "rtx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::restCategory));

	// ".restTranslateY" attribute
	//
	PathConstraint::restTranslateY = fnUnitAttr.create("restTranslateY", "rty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::restCategory));

	// ".restTranslateZ" attribute
	//
	PathConstraint::restTranslateZ = fnUnitAttr.create("restTranslateZ", "rtz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::restCategory));

	// ".restTranslate" attribute
	//
	PathConstraint::restTranslate = fnNumericAttr.create("restTranslate", "rt", PathConstraint::restTranslateX, PathConstraint::restTranslateY, PathConstraint::restTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::restCategory));

	// ".restRotateX" attribute
	//
	PathConstraint::restRotateX = fnUnitAttr.create("restRotateX", "rrx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::restCategory));

	// ".restRotateY" attribute
	//
	PathConstraint::restRotateY = fnUnitAttr.create("restRotateY", "rry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::restCategory));

	// ".restRotateZ" attribute
	//
	PathConstraint::restRotateZ = fnUnitAttr.create("restRotateZ", "rrz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::restCategory));

	// ".restRotate" attribute
	//
	PathConstraint::restRotate = fnNumericAttr.create("restRotate", "rr", PathConstraint::restRotateX, PathConstraint::restRotateY, PathConstraint::restRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::restCategory));

	// ".constraintRotateOrder" attribute
	//
	PathConstraint::constraintRotateOrder = fnEnumAttr.create("constraintRotateOrder", "cro", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 5));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PathConstraint::inputCategory));

	// ".constraintParentInverseMatrix" attribute
	//
	PathConstraint::constraintParentInverseMatrix = fnMatrixAttr.create("constraintParentInverseMatrix", "cpim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PathConstraint::inputCategory));

	// Target Attributes:
	// ".targetWeight" attribute
	//
	PathConstraint::targetWeight = fnNumericAttr.create("targetWeight", "tw", MFnNumericData::kFloat, 50.0f, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0f));
	CHECK_MSTATUS(fnNumericAttr.setMax(100.0f));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::targetCategory));

	// ".targetCurve" attribute
	//
	PathConstraint::targetCurve = fnTypedAttr.create("targetCurve", "tc", MFnData::kNurbsCurve, MObject::kNullObj, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(PathConstraint::targetCategory));

	// ".target" attribute
	//
	PathConstraint::target = fnCompoundAttr.create("target", "target", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(PathConstraint::targetWeight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(PathConstraint::targetCurve));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(PathConstraint::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(PathConstraint::targetCategory));

	// Output attributes:
	// ".constraintTranslateX" attribute
	//
	PathConstraint::constraintTranslateX = fnUnitAttr.create("constraintTranslateX", "ctx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintTranslateY" attribute
	//
	PathConstraint::constraintTranslateY = fnUnitAttr.create("constraintTranslateY", "cty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintTranslateZ" attribute
	//
	PathConstraint::constraintTranslateZ = fnUnitAttr.create("constraintTranslateZ", "ctz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintTranslate" attribute
	//
	PathConstraint::constraintTranslate = fnNumericAttr.create("constraintTranslate", "ct", PathConstraint::constraintTranslateX, PathConstraint::constraintTranslateY, PathConstraint::constraintTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintRotateX" attribute
	//
	PathConstraint::constraintRotateX = fnUnitAttr.create("constraintRotateX", "crx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintRotateY" attribute
	//
	PathConstraint::constraintRotateY = fnUnitAttr.create("constraintRotateY", "cry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintRotateZ" attribute
	//
	PathConstraint::constraintRotateZ = fnUnitAttr.create("constraintRotateZ", "crz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintRotate" attribute
	//
	PathConstraint::constraintRotate = fnNumericAttr.create("constraintRotate", "cr", PathConstraint::constraintRotateX, PathConstraint::constraintRotateY, PathConstraint::constraintRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintMatrix" attribute
	//
	PathConstraint::constraintMatrix = fnMatrixAttr.create("constraintMatrix", "cm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintInverseMatrix" attribute
	//
	PathConstraint::constraintInverseMatrix = fnMatrixAttr.create("constraintInverseMatrix", "cim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintWorldMatrix" attribute
	//
	PathConstraint::constraintWorldMatrix = fnMatrixAttr.create("constraintWorldMatrix", "cwm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PathConstraint::outputCategory));

	// ".constraintWorldInverseMatrix" attribute
	//
	PathConstraint::constraintWorldInverseMatrix = fnMatrixAttr.create("constraintWorldInverseMatrix", "cwim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PathConstraint::outputCategory));

	// Add attributes
	//
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::percent));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::useParameter));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::loop));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::forwardAxis));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::forwardAxisFlip));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::twist));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::upAxis));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::upAxisFlip));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::worldUpType));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::worldUpVector));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::worldUpMatrix));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::restTranslate));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::restRotate));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::relative));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::offsetTranslate));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::offsetRotate));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::target));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintTranslate));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintRotateOrder));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintRotate));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintMatrix));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintInverseMatrix));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintWorldMatrix));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintWorldInverseMatrix));
	CHECK_MSTATUS(PathConstraint::addAttribute(PathConstraint::constraintParentInverseMatrix));

	// Define attribute relationships
	//
	MObjectArray inputs, outputs;

	status = Maxformations::getAttributesByCategory(PathConstraint::id, PathConstraint::inputCategory, inputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::getAttributesByCategory(PathConstraint::id, PathConstraint::outputCategory, outputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	unsigned int inputCount = inputs.length();
	unsigned int outputCount = outputs.length();

	for (unsigned int i = 0; i < inputCount; i++)
	{

		for (unsigned int j = 0; j < outputCount; j++)
		{

			CHECK_MSTATUS(PathConstraint::attributeAffects(inputs[i], outputs[j]));

		}

	}
	
	return status;

};