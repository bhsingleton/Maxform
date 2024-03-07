//
// File: LookAtConstraint.cpp
//
// Dependency Graph Node: LookAtConstraint
//
// Author: Ben Singleton
//

#include "LookAtConstraint.h"


MObject LookAtConstraint::targetAxis;
MObject LookAtConstraint::targetAxisFlip;
MObject LookAtConstraint::roll;
MObject LookAtConstraint::sourceUpAxis;
MObject LookAtConstraint::sourceUpAxisFlip;
MObject LookAtConstraint::upNode;
MObject LookAtConstraint::upNodeControl;
MObject LookAtConstraint::upNodeWorld;
MObject LookAtConstraint::upNodeAxis;
MObject LookAtConstraint::target;
MObject LookAtConstraint::targetWeight;
MObject LookAtConstraint::targetMatrix;
MObject LookAtConstraint::targetParentMatrix;
MObject LookAtConstraint::relative;
MObject LookAtConstraint::offsetRotate;
MObject LookAtConstraint::offsetRotateX;
MObject LookAtConstraint::offsetRotateY;
MObject LookAtConstraint::offsetRotateZ;
MObject LookAtConstraint::restRotate;
MObject LookAtConstraint::restRotateX;
MObject LookAtConstraint::restRotateY;
MObject LookAtConstraint::restRotateZ;

MObject LookAtConstraint::constraintTranslate;
MObject LookAtConstraint::constraintTranslateX;
MObject LookAtConstraint::constraintTranslateY;
MObject LookAtConstraint::constraintTranslateZ;
MObject LookAtConstraint::constraintRotateOrder;
MObject LookAtConstraint::constraintRotate;
MObject LookAtConstraint::constraintRotateX;
MObject LookAtConstraint::constraintRotateY;
MObject LookAtConstraint::constraintRotateZ;
MObject LookAtConstraint::constraintMatrix;
MObject LookAtConstraint::constraintInverseMatrix;
MObject LookAtConstraint::constraintWorldMatrix;
MObject LookAtConstraint::constraintWorldInverseMatrix;
MObject LookAtConstraint::constraintParentInverseMatrix;

MString	LookAtConstraint::inputCategory("Input");
MString	LookAtConstraint::offsetCategory("Offset");
MString	LookAtConstraint::restCategory("Rest");
MString	LookAtConstraint::targetCategory("Target");
MString	LookAtConstraint::outputCategory("Output");

MString LookAtConstraint::classification("animation");

MTypeId	LookAtConstraint::id(0x0013b1d7);


LookAtConstraint::LookAtConstraint() {};
LookAtConstraint::~LookAtConstraint() {};


MStatus LookAtConstraint::compute(const MPlug& plug, MDataBlock& data)
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

	if (fnAttribute.hasCategory(LookAtConstraint::outputCategory))
	{

		// Get input data handles
		//
		MDataHandle targetAxisHandle = data.inputValue(LookAtConstraint::targetAxis, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle targetAxisFlipHandle = data.inputValue(LookAtConstraint::targetAxisFlip, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rollHandle = data.inputValue(LookAtConstraint::roll, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle sourceUpAxisHandle = data.inputValue(LookAtConstraint::sourceUpAxis, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle sourceUpAxisFlipHandle = data.inputValue(LookAtConstraint::sourceUpAxisFlip, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle upNodeHandle = data.inputValue(LookAtConstraint::upNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle upNodeWorldHandle = data.inputValue(LookAtConstraint::upNodeWorld, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle upNodeControlHandle = data.inputValue(LookAtConstraint::upNodeControl, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle upNodeAxisHandle = data.inputValue(LookAtConstraint::upNodeAxis, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle relativeHandle = data.inputValue(LookAtConstraint::relative, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle offsetRotateHandle = data.inputValue(LookAtConstraint::offsetRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle enableRestPositionHandle = data.inputValue(LookAtConstraint::enableRestPosition, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle restRotateHandle = data.inputValue(LookAtConstraint::restRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintTranslateHandle = data.inputValue(LookAtConstraint::constraintTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateOrderHandle = data.inputValue(LookAtConstraint::constraintRotateOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintParentInverseMatrixHandle = data.inputValue(LookAtConstraint::constraintParentInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle targetArrayHandle = data.inputArrayValue(LookAtConstraint::target, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		int targetAxis = targetAxisHandle.asShort();
		bool targetAxisFlip = targetAxisFlipHandle.asBool();
		MAngle roll = rollHandle.asAngle();
		int sourceUpAxis = sourceUpAxisHandle.asShort();
		bool sourceUpAxisFlip = sourceUpAxisFlipHandle.asBool();
		bool upNodeWorld = upNodeWorldHandle.asBool();
		int upNodeControl = upNodeControlHandle.asShort();
		MMatrix upNodeMatrix = upNodeHandle.asMatrix();
		int upNodeAxis = upNodeAxisHandle.asShort();
		UpNodeSettings upNodeSettings = { upNodeMatrix, upNodeWorld, upNodeControl, upNodeAxis };
		
		MEulerRotation::RotationOrder constraintRotateOrder = MEulerRotation::RotationOrder(constraintRotateOrderHandle.asShort());
		MMatrix constraintParentInverseMatrix = constraintParentInverseMatrixHandle.asMatrix();
		MMatrix constraintParentMatrix = constraintParentInverseMatrix.inverse();
		MVector constraintTranslate = constraintTranslateHandle.asVector();
		MMatrix constraintTranslateMatrix = Maxformations::createPositionMatrix(constraintTranslate);
		MVector constraintOrigin = Maxformations::matrixToPosition(constraintTranslateMatrix * constraintParentMatrix);
		
		bool relative = relativeHandle.asBool();
		MVector offsetRotate = offsetRotateHandle.asVector();
		MMatrix offsetRotateMatrix = Maxformations::createRotationMatrix(offsetRotate);
		MMatrix offsetMatrix = relative ? offsetRotateMatrix : MMatrix::identity;

		bool restEnabled = enableRestPositionHandle.asBool();
		MVector restRotate = restRotateHandle.asVector();
		MMatrix restRotateMatrix = Maxformations::createRotationMatrix(restRotate, constraintRotateOrder);
		MMatrix restMatrix = restEnabled ? restRotateMatrix : MMatrix::identity;
		MMatrix restWorldMatrix = restMatrix * constraintParentMatrix;

		// Calculate up-vector
		//
		MVector upVector;

		status = LookAtConstraint::getUpVector(upNodeSettings, constraintOrigin, upVector);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Compose roll matrix
		//
		MMatrix rollMatrix;

		status = Maxformations::createTwistMatrix(targetAxis, roll, rollMatrix);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Collect target matrices
		//
		unsigned int targetCount = targetArrayHandle.elementCount();
		MFloatArray targetWeights = MFloatArray(targetCount);
		MMatrixArray targetMatrices = MMatrixArray(targetCount);

		MDataHandle targetHandle, targetWeightHandle, targetMatrixHandle, targetParentMatrixHandle;
		MMatrix targetMatrix, targetParentMatrix, targetWorldMatrix;

		for (unsigned int i = 0; i < targetCount; i++)
		{

			// Jump to array element
			//
			status = targetArrayHandle.jumpToArrayElement(i);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			targetHandle = targetArrayHandle.inputValue(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get target data handles
			//
			targetWeightHandle = targetHandle.child(LookAtConstraint::targetWeight);
			targetMatrixHandle = targetHandle.child(LookAtConstraint::targetMatrix);
			targetParentMatrixHandle = targetHandle.child(LookAtConstraint::targetParentMatrix);

			// Get target weight
			//
			targetWeights[i] = Maxformations::clamp(targetWeightHandle.asFloat(), 0.0f, 100.0f) / 100.0f;

			// Get target matrices
			//
			targetMatrix = targetMatrixHandle.asMatrix();
			targetParentMatrix = targetParentMatrixHandle.asMatrix();
			targetWorldMatrix = targetMatrix * targetParentMatrix;

			targetMatrices[i] = targetWorldMatrix;

		}

		// Calculate aim matrix
		// If there are no weights then default back to rest matrix!
		//
		float weightSum = Maxformations::sum(targetWeights);
		MMatrix constraintWorldMatrix, constraintMatrix;

		if (weightSum > 0.0)
		{

			MMatrix blendMatrix = Maxformations::blendMatrices(restWorldMatrix, targetMatrices, targetWeights);
			MVector targetPoint = Maxformations::matrixToPosition(blendMatrix);
			MVector forwardVector = (targetPoint - constraintOrigin).normal();

			MMatrix aimMatrix;

			status = Maxformations::createAimMatrix(forwardVector, targetAxis, targetAxisFlip, upVector, sourceUpAxis, sourceUpAxisFlip, constraintOrigin, aimMatrix);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			constraintWorldMatrix = offsetMatrix * (rollMatrix * aimMatrix);
			constraintMatrix = constraintWorldMatrix * constraintParentInverseMatrix;

		}
		else
		{

			constraintWorldMatrix = restWorldMatrix;
			constraintMatrix = constraintWorldMatrix * constraintParentInverseMatrix;

		}

		// Get output data handles
		//
		MDataHandle constraintRotateXHandle = data.outputValue(LookAtConstraint::constraintRotateX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateYHandle = data.outputValue(LookAtConstraint::constraintRotateY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateZHandle = data.outputValue(LookAtConstraint::constraintRotateZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintMatrixHandle = data.outputValue(LookAtConstraint::constraintMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintInverseMatrixHandle = data.outputValue(LookAtConstraint::constraintInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldMatrixHandle = data.outputValue(LookAtConstraint::constraintWorldMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldInverseMatrixHandle = data.outputValue(LookAtConstraint::constraintWorldInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		MEulerRotation constraintRotate = Maxformations::matrixToEulerRotation(constraintMatrix, constraintRotateOrder);

		constraintRotateXHandle.setMAngle(MAngle(constraintRotate.x, MAngle::kRadians));
		constraintRotateYHandle.setMAngle(MAngle(constraintRotate.y, MAngle::kRadians));
		constraintRotateZHandle.setMAngle(MAngle(constraintRotate.z, MAngle::kRadians));

		constraintRotateXHandle.setClean();
		constraintRotateYHandle.setClean();
		constraintRotateZHandle.setClean();

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


MStatus LookAtConstraint::getUpVector(const UpNodeSettings& settings, const MVector& origin, MVector& upVector)
/**
Returns the up-vector based on the supplied up-node settings.

@param settings: The up-node settings.
@param origin: The origin of the look-at.
@param upVector: The passed vector to populate.
@return: Return status.
*/
{

	MStatus status;

	switch (settings.upNodeControl)
	{

	case 0:  // Look-At
	{

		if (settings.upNodeWorld)
		{

			upVector = (MVector::zero - origin).normal();

		}
		else
		{

			MVector target = Maxformations::matrixToPosition(settings.upNodeMatrix);
			upVector = (target - origin).normal();

		}

	}
	break;

	case 1:  // Axis Alignment
	{

		if (settings.upNodeWorld)
		{

			upVector = Maxformations::getAxisVector(settings.upNodeAxis, false);

		}
		else
		{

			upVector = Maxformations::getAxisVector(settings.upNodeMatrix, settings.upNodeAxis, false);

		}

	}
	break;

	default:
	{

		return MS::kFailure;

	}
	break;

	}

	return MS::kSuccess;;

};


const MObject LookAtConstraint::targetAttribute() const
/**
Returns the target attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return LookAtConstraint::target;

};


const MObject LookAtConstraint::weightAttribute() const
/**
Returns the weight attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return LookAtConstraint::targetWeight;

};


const MObject LookAtConstraint::constraintRotateOrderAttribute() const
/**
Returns the rotate order attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return LookAtConstraint::constraintRotateOrder;

};


void* LookAtConstraint::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: TransformConstraint
*/
{

	return new LookAtConstraint();

};


MStatus LookAtConstraint::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{

	MStatus status;

	// Declare attribute function sets
	//
	MFnCompoundAttribute fnCompoundAttr;
	MFnNumericAttribute fnNumericAttr;
	MFnUnitAttribute fnUnitAttr;
	MFnEnumAttribute fnEnumAttr;
	MFnMatrixAttribute fnMatrixAttr;

	// Input attributes:
	// ".targetAxis" attribute
	//
	LookAtConstraint::targetAxis = fnEnumAttr.create("targetAxis", "ta", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("X", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("Y", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("Z", 2));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".targetAxisFlip" attribute
	//
	LookAtConstraint::targetAxisFlip = fnNumericAttr.create("targetAxisFlip", "taf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".roll" attribute
	//
	LookAtConstraint::roll = fnUnitAttr.create("roll", "rl", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".sourceUpAxis" attribute
	//
	LookAtConstraint::sourceUpAxis = fnEnumAttr.create("sourceUpAxis", "sua", short(2), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("X", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("Y", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("Z", 2));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".sourceUpAxisFlip" attribute
	//
	LookAtConstraint::sourceUpAxisFlip = fnNumericAttr.create("sourceUpAxisFlip", "suaf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".upNode" attribute
	//
	LookAtConstraint::upNode = fnMatrixAttr.create("upNode", "un", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".upNodeWorld" attribute
	//
	LookAtConstraint::upNodeWorld = fnNumericAttr.create("upNodeWorld", "unw", MFnNumericData::kBoolean, true, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".upNodeControl" attribute
	//
	LookAtConstraint::upNodeControl = fnEnumAttr.create("upNodeControl", "unc", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnEnumAttr.addField("Look-At", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("Axis Alignment", 1));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".upNodeAxis" attribute
	//
	LookAtConstraint::upNodeAxis = fnEnumAttr.create("upNodeAxis", "una", short(2), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("X", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("Y", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("Z", 2));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".relative" attribute
	//
	LookAtConstraint::relative = fnNumericAttr.create("relative", "rel", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".offsetRotateX" attribute
	//
	LookAtConstraint::offsetRotateX = fnUnitAttr.create("offsetRotateX", "orx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::offsetCategory));

	// ".offsetRotateY" attribute
	//
	LookAtConstraint::offsetRotateY = fnUnitAttr.create("offsetRotateY", "ory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::offsetCategory));

	// ".offsetRotateZ" attribute
	//
	LookAtConstraint::offsetRotateZ = fnUnitAttr.create("offsetRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::offsetCategory));

	// ".offsetRotate" attribute
	//
	LookAtConstraint::offsetRotate = fnNumericAttr.create("offsetRotate", "or", LookAtConstraint::offsetRotateX, LookAtConstraint::offsetRotateY, LookAtConstraint::offsetRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::offsetCategory));

	// ".enableRestPosition" attribute
	//
	status = fnNumericAttr.setObject(LookAtConstraint::enableRestPosition);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::restCategory));

	// ".restRotateX" attribute
	//
	LookAtConstraint::restRotateX = fnUnitAttr.create("restRotateX", "rrx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::restCategory));

	// ".restRotateY" attribute
	//
	LookAtConstraint::restRotateY = fnUnitAttr.create("restRotateY", "rry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::restCategory));

	// ".restRotateZ" attribute
	//
	LookAtConstraint::restRotateZ = fnUnitAttr.create("restRotateZ", "rrz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::restCategory));

	// ".restRotate" attribute
	//
	LookAtConstraint::restRotate = fnNumericAttr.create("restRotate", "rr", LookAtConstraint::restRotateX, LookAtConstraint::restRotateY, LookAtConstraint::restRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::restCategory));

	// ".constraintTranslateX" attribute
	//
	LookAtConstraint::constraintTranslateX = fnUnitAttr.create("constraintTranslateX", "ctx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".constraintTranslateY" attribute
	//
	LookAtConstraint::constraintTranslateY = fnUnitAttr.create("constraintTranslateY", "cty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".constraintTranslateZ" attribute
	//
	LookAtConstraint::constraintTranslateZ = fnUnitAttr.create("constraintTranslateZ", "ctz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".constraintTranslate" attribute
	//
	LookAtConstraint::constraintTranslate = fnNumericAttr.create("constraintTranslate", "ct", LookAtConstraint::constraintTranslateX, LookAtConstraint::constraintTranslateY, LookAtConstraint::constraintTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".constraintRotateOrder" attribute
	//
	LookAtConstraint::constraintRotateOrder = fnEnumAttr.create("constraintRotateOrder", "cro", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 5));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(LookAtConstraint::inputCategory));

	// ".constraintParentInverseMatrix" attribute
	//
	LookAtConstraint::constraintParentInverseMatrix = fnMatrixAttr.create("constraintParentInverseMatrix", "cpim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::inputCategory));

	// Target Attributes:
	// Define ".targetWeight" attribute
	//
	LookAtConstraint::targetWeight = fnNumericAttr.create("targetWeight", "tw", MFnNumericData::kFloat, 50.0f, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0f));
	CHECK_MSTATUS(fnNumericAttr.setMax(100.0f));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::targetCategory));

	// ".targetMatrix" attribute
	//
	LookAtConstraint::targetMatrix = fnMatrixAttr.create("targetMatrix", "tm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::targetCategory));

	// ".targetParentMatrix" attribute
	//
	LookAtConstraint::targetParentMatrix = fnMatrixAttr.create("targetParentMatrix", "tpm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::targetCategory));

	// Define ".target" attribute
	//
	LookAtConstraint::target = fnCompoundAttr.create("target", "tg", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(LookAtConstraint::targetWeight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(LookAtConstraint::targetMatrix));
	CHECK_MSTATUS(fnCompoundAttr.addChild(LookAtConstraint::targetParentMatrix));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(LookAtConstraint::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(LookAtConstraint::targetCategory));

	// Output attributes:
	// Define ".constraintRotateX" attribute
	//
	LookAtConstraint::constraintRotateX = fnUnitAttr.create("constraintRotateX", "crx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::outputCategory));

	// ".constraintRotateY" attribute
	//
	LookAtConstraint::constraintRotateY = fnUnitAttr.create("constraintRotateY", "cry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::outputCategory));

	// ".constraintRotateZ" attribute
	//
	LookAtConstraint::constraintRotateZ = fnUnitAttr.create("constraintRotateZ", "crz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(LookAtConstraint::outputCategory));

	// ".constraintRotate" attribute
	//
	LookAtConstraint::constraintRotate = fnNumericAttr.create("constraintRotate", "cr", LookAtConstraint::constraintRotateX, LookAtConstraint::constraintRotateY, LookAtConstraint::constraintRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(LookAtConstraint::outputCategory));

	// ".constraintMatrix" attribute
	//
	LookAtConstraint::constraintMatrix = fnMatrixAttr.create("constraintMatrix", "cm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::outputCategory));

	// ".constraintInverseMatrix" attribute
	//
	LookAtConstraint::constraintInverseMatrix = fnMatrixAttr.create("constraintInverseMatrix", "cim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::outputCategory));
	
	// ".constraintWorldMatrix" attribute
	//
	LookAtConstraint::constraintWorldMatrix = fnMatrixAttr.create("constraintWorldMatrix", "cwm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::outputCategory));

	// ".constraintWorldInverseMatrix" attribute
	//
	LookAtConstraint::constraintWorldInverseMatrix = fnMatrixAttr.create("constraintWorldInverseMatrix", "cwim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(LookAtConstraint::outputCategory));

	// Add attributes
	//
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::targetAxis));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::targetAxisFlip));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::roll));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::sourceUpAxis));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::sourceUpAxisFlip));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::upNode));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::upNodeWorld));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::upNodeControl));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::upNodeAxis));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::restRotate));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::relative));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::offsetRotate));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::target));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintTranslate));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintRotateOrder));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintRotate));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintMatrix));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintInverseMatrix));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintWorldMatrix));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintWorldInverseMatrix));
	CHECK_MSTATUS(LookAtConstraint::addAttribute(LookAtConstraint::constraintParentInverseMatrix));

	// Define attribute relationships
	//
	MObjectArray inputs, outputs;

	status = Maxformations::getAttributesByCategory(LookAtConstraint::id, LookAtConstraint::inputCategory, inputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::getAttributesByCategory(LookAtConstraint::id, LookAtConstraint::outputCategory, outputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	unsigned int inputCount = inputs.length();
	unsigned int outputCount = outputs.length();

	for (unsigned int i = 0; i < inputCount; i++)
	{

		for (unsigned int j = 0; j < outputCount; j++)
		{

			CHECK_MSTATUS(LookAtConstraint::attributeAffects(inputs[i], outputs[j]));

		}

	}

	return MS::kSuccess;

};
