//
// File: OrientationConstraint.cpp
//
// Dependency Graph Node: OrientationConstraint
//
// Author: Ben Singleton
//

#include "OrientationConstraint.h"


MObject OrientationConstraint::localOrWorld;
MObject OrientationConstraint::relative;
MObject OrientationConstraint::offsetRotate;
MObject OrientationConstraint::offsetRotateX;
MObject OrientationConstraint::offsetRotateY;
MObject OrientationConstraint::offsetRotateZ;
MObject OrientationConstraint::restRotate;
MObject OrientationConstraint::restRotateX;
MObject OrientationConstraint::restRotateY;
MObject OrientationConstraint::restRotateZ;
MObject OrientationConstraint::target;
MObject OrientationConstraint::targetWeight;
MObject OrientationConstraint::targetMatrix;
MObject OrientationConstraint::targetParentMatrix;
MObject OrientationConstraint::targetOffsetRotate;
MObject OrientationConstraint::targetOffsetRotateX;
MObject OrientationConstraint::targetOffsetRotateY;
MObject OrientationConstraint::targetOffsetRotateZ;

MObject OrientationConstraint::constraintRotateOrder;
MObject OrientationConstraint::constraintRotate;
MObject OrientationConstraint::constraintRotateX;
MObject OrientationConstraint::constraintRotateY;
MObject OrientationConstraint::constraintRotateZ;
MObject OrientationConstraint::constraintMatrix;
MObject OrientationConstraint::constraintInverseMatrix;
MObject OrientationConstraint::constraintWorldMatrix;
MObject OrientationConstraint::constraintWorldInverseMatrix;
MObject OrientationConstraint::constraintParentInverseMatrix;

MString	OrientationConstraint::inputCategory("Input");
MString	OrientationConstraint::offsetCategory("Offset");
MString	OrientationConstraint::restCategory("Rest");
MString	OrientationConstraint::targetCategory("Target");
MString	OrientationConstraint::outputCategory("Output");

MString OrientationConstraint::classification("animation");

MTypeId	OrientationConstraint::id(0x0013b1d6);


OrientationConstraint::OrientationConstraint() {};
OrientationConstraint::~OrientationConstraint() {};


MStatus OrientationConstraint::compute(const MPlug& plug, MDataBlock& data)
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

	if (fnAttribute.hasCategory(OrientationConstraint::outputCategory))
	{

		// Get input data handles
		//
		MDataHandle relativeHandle = data.inputValue(OrientationConstraint::relative, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle offsetRotateHandle = data.inputValue(OrientationConstraint::offsetRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localOrWorldHandle = data.inputValue(OrientationConstraint::localOrWorld, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle enableRestPositionHandle = data.inputValue(OrientationConstraint::enableRestPosition, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle restRotateHandle = data.inputValue(OrientationConstraint::restRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateOrderHandle = data.inputValue(OrientationConstraint::constraintRotateOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintParentInverseMatrixHandle = data.inputValue(OrientationConstraint::constraintParentInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle targetArrayHandle = data.inputArrayValue(OrientationConstraint::target, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		bool localOrWorld = localOrWorldHandle.asBool();
		bool relative = relativeHandle.asBool();
		MVector offsetRotate = offsetRotateHandle.asVector();
		MMatrix offsetMatrix = relative ? Maxformations::createRotationMatrix(offsetRotate) : MMatrix::identity;
		
		MEulerRotation::RotationOrder constraintRotateOrder = MEulerRotation::RotationOrder(constraintRotateOrderHandle.asShort());
		MMatrix constraintParentInverseMatrix = constraintParentInverseMatrixHandle.asMatrix();
		MMatrix constraintParentMatrix = constraintParentInverseMatrix.inverse();

		bool restEnabled = enableRestPositionHandle.asBool();
		MVector restRotate = restRotateHandle.asVector();
		MMatrix restRotateMatrix = Maxformations::createRotationMatrix(restRotate, constraintRotateOrder);
		MMatrix restMatrix = restEnabled ? restRotateMatrix : MMatrix::identity;
		MMatrix restWorldMatrix = restMatrix * constraintParentMatrix;

		// Collect target matrices
		//
		unsigned int targetCount = targetArrayHandle.elementCount();

		MFloatArray targetWeights = MFloatArray(targetCount);
		MMatrixArray targetMatrices = MMatrixArray(targetCount);
		MMatrixArray targetWorldMatrices = MMatrixArray(targetCount);

		MDataHandle targetHandle, targetWeightHandle, targetMatrixHandle, targetParentMatrixHandle, targetOffsetRotateHandle;
		MMatrix targetMatrix, targetParentMatrix, targetOffsetMatrix;
		
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
			targetWeightHandle = targetHandle.child(OrientationConstraint::targetWeight);
			targetMatrixHandle = targetHandle.child(OrientationConstraint::targetMatrix);
			targetParentMatrixHandle = targetHandle.child(OrientationConstraint::targetParentMatrix);
			targetOffsetRotateHandle = targetHandle.child(OrientationConstraint::targetOffsetRotate);

			// Get target weight
			//
			targetWeights[i] = Maxformations::clamp(targetWeightHandle.asFloat(), 0.0f, 100.0f) / 100.0f;

			// Get target matrices
			//
			targetMatrix = targetMatrixHandle.asMatrix();
			targetParentMatrix = targetParentMatrixHandle.asMatrix();
			targetOffsetMatrix = relative ? Maxformations::createRotationMatrix(targetOffsetRotateHandle.asVector()) : MMatrix::identity;

			targetMatrices[i] = Maxformations::createRotationMatrix(targetOffsetMatrix * Maxformations::normalizeMatrix(targetMatrix));
			targetWorldMatrices[i] = Maxformations::createRotationMatrix(targetOffsetMatrix * Maxformations::normalizeMatrix(targetMatrix * targetParentMatrix));

		}

		// Calculate weighted constraint matrix
		//
		MMatrix orientationMatrix, constraintMatrix, constraintWorldMatrix;

		if (localOrWorld)  // Local
		{

			orientationMatrix = Maxformations::blendMatrices(restMatrix, targetMatrices, targetWeights);

			constraintMatrix = offsetMatrix * orientationMatrix;
			constraintWorldMatrix = constraintMatrix * constraintParentMatrix;

		}
		else  // World
		{

			orientationMatrix = Maxformations::blendMatrices(restWorldMatrix, targetWorldMatrices, targetWeights);

			constraintWorldMatrix = offsetMatrix * orientationMatrix;
			constraintMatrix = constraintWorldMatrix * constraintParentInverseMatrix;

		}

		// Get output data handles
		//
		MDataHandle constraintRotateXHandle = data.outputValue(OrientationConstraint::constraintRotateX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateYHandle = data.outputValue(OrientationConstraint::constraintRotateY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateZHandle = data.outputValue(OrientationConstraint::constraintRotateZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintMatrixHandle = data.outputValue(OrientationConstraint::constraintMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintInverseMatrixHandle = data.outputValue(OrientationConstraint::constraintInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldMatrixHandle = data.outputValue(OrientationConstraint::constraintWorldMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldInverseMatrixHandle = data.outputValue(OrientationConstraint::constraintWorldInverseMatrix, &status);
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

}


const MObject OrientationConstraint::targetAttribute() const
/**
Returns the target attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return OrientationConstraint::target;

};


const MObject OrientationConstraint::weightAttribute() const
/**
Returns the weight attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return OrientationConstraint::targetWeight;

};


const MObject OrientationConstraint::constraintRotateOrderAttribute() const
/**
Returns the rotate order attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return OrientationConstraint::constraintRotateOrder;

};


void* OrientationConstraint::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: TransformConstraint
*/
{

	return new OrientationConstraint();

};


MStatus OrientationConstraint::initialize()
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
	// ".localOrWorld" attribute
	//
	OrientationConstraint::localOrWorld = fnNumericAttr.create("localOrWorld", "low", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));

	// ".relative" attribute
	//
	OrientationConstraint::relative = fnNumericAttr.create("relative", "rel", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));

	// ".offsetRotateX" attribute
	//
	OrientationConstraint::offsetRotateX = fnUnitAttr.create("offsetRotateX", "orx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::offsetCategory));

	// ".offsetRotateY" attribute
	//
	OrientationConstraint::offsetRotateY = fnUnitAttr.create("offsetRotateY", "ory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::offsetCategory));

	// ".offsetRotateZ" attribute
	//
	OrientationConstraint::offsetRotateZ = fnUnitAttr.create("offsetRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::offsetCategory));

	// ".offsetRotate" attribute
	//
	OrientationConstraint::offsetRotate = fnNumericAttr.create("offsetRotate", "or", OrientationConstraint::offsetRotateX, OrientationConstraint::offsetRotateY, OrientationConstraint::offsetRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::offsetCategory));

	// ".enableRestPosition" attribute
	//
	status = fnNumericAttr.setObject(OrientationConstraint::enableRestPosition);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::restCategory));

	// ".restRotateX" attribute
	//
	OrientationConstraint::restRotateX = fnUnitAttr.create("restRotateX", "rrx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::restCategory));

	// ".restRotateY" attribute
	//
	OrientationConstraint::restRotateY = fnUnitAttr.create("restRotateY", "rry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::restCategory));

	// ".restRotateZ" attribute
	//
	OrientationConstraint::restRotateZ = fnUnitAttr.create("restRotateZ", "rrz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::restCategory));

	// ".restRotate" attribute
	//
	OrientationConstraint::restRotate = fnNumericAttr.create("restRotate", "rr", OrientationConstraint::restRotateX, OrientationConstraint::restRotateY, OrientationConstraint::restRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::restCategory));

	// ".constraintRotateOrder" attribute
	//
	OrientationConstraint::constraintRotateOrder = fnEnumAttr.create("constraintRotateOrder", "cro", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 5));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(OrientationConstraint::inputCategory));

	// ".constraintParentInverseMatrix" attribute
	//
	OrientationConstraint::constraintParentInverseMatrix = fnMatrixAttr.create("constraintParentInverseMatrix", "cpim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(OrientationConstraint::inputCategory));

	// Target Attributes:
	// ".targetWeight" attribute
	//
	OrientationConstraint::targetWeight = fnNumericAttr.create("targetWeight", "tw", MFnNumericData::kFloat, 50.0f, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0f));
	CHECK_MSTATUS(fnNumericAttr.setMax(100.0f));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::targetCategory));

	// ".targetMatrix" attribute
	//
	OrientationConstraint::targetMatrix = fnMatrixAttr.create("targetMatrix", "tm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::targetCategory));

	// ".targetParentMatrix" attribute
	//
	OrientationConstraint::targetParentMatrix = fnMatrixAttr.create("targetParentMatrix", "tpm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::targetCategory));

	// ".targetOffsetRotateX" attribute
	//
	OrientationConstraint::targetOffsetRotateX = fnUnitAttr.create("targetOffsetRotateX", "torx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::offsetCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::targetCategory));

	// ".targetOffsetRotateY" attribute
	//
	OrientationConstraint::targetOffsetRotateY = fnUnitAttr.create("targetOffsetRotateY", "tory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::offsetCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::targetCategory));

	// ".targetOffsetRotateZ" attribute
	//
	OrientationConstraint::targetOffsetRotateZ = fnUnitAttr.create("targetOffsetRotateZ", "torz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::offsetCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::targetCategory));

	// ".targetOffsetRotate" attribute
	//
	OrientationConstraint::targetOffsetRotate = fnNumericAttr.create("targetOffsetRotate", "tor", OrientationConstraint::targetOffsetRotateX, OrientationConstraint::targetOffsetRotateY, OrientationConstraint::targetOffsetRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::offsetCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::targetCategory));

	// ".target" attribute
	//
	OrientationConstraint::target = fnCompoundAttr.create("target", "tg", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(OrientationConstraint::targetWeight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(OrientationConstraint::targetMatrix));
	CHECK_MSTATUS(fnCompoundAttr.addChild(OrientationConstraint::targetParentMatrix));
	CHECK_MSTATUS(fnCompoundAttr.addChild(OrientationConstraint::targetOffsetRotate));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(OrientationConstraint::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(OrientationConstraint::targetCategory));

	// Output attributes:
	// ".constraintRotateX" attribute
	//
	OrientationConstraint::constraintRotateX = fnUnitAttr.create("constraintRotateX", "ctx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::outputCategory));

	// ".constraintRotateY" attribute
	//
	OrientationConstraint::constraintRotateY = fnUnitAttr.create("constraintRotateY", "cty", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::outputCategory));

	// ".constraintRotateZ" attribute
	//
	OrientationConstraint::constraintRotateZ = fnUnitAttr.create("constraintRotateZ", "ctz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(OrientationConstraint::outputCategory));

	// ".constraintRotate" attribute
	//
	OrientationConstraint::constraintRotate = fnNumericAttr.create("constraintRotate", "ct", OrientationConstraint::constraintRotateX, OrientationConstraint::constraintRotateY, OrientationConstraint::constraintRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(OrientationConstraint::outputCategory));

	// ".constraintMatrix" attribute
	//
	OrientationConstraint::constraintMatrix = fnMatrixAttr.create("constraintMatrix", "cm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(OrientationConstraint::outputCategory));

	// ".constraintInverseMatrix" attribute
	//
	OrientationConstraint::constraintInverseMatrix = fnMatrixAttr.create("constraintInverseMatrix", "cim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(OrientationConstraint::outputCategory));

	// ".constraintWorldMatrix" attribute
	//
	OrientationConstraint::constraintWorldMatrix = fnMatrixAttr.create("constraintWorldMatrix", "cwm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(OrientationConstraint::outputCategory));

	// ".constraintWorldInverseMatrix" attribute
	//
	OrientationConstraint::constraintWorldInverseMatrix = fnMatrixAttr.create("constraintWorldInverseMatrix", "cwim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(OrientationConstraint::outputCategory));

	// Add attributes
	//
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::localOrWorld));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::relative));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::offsetRotate));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::restRotate));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::target));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::constraintRotateOrder));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::constraintRotate));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::constraintMatrix));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::constraintInverseMatrix));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::constraintWorldMatrix));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::constraintWorldInverseMatrix));
	CHECK_MSTATUS(OrientationConstraint::addAttribute(OrientationConstraint::constraintParentInverseMatrix));

	// Define attribute relationships
	//
	MObjectArray inputs, outputs;

	status = Maxformations::getAttributesByCategory(OrientationConstraint::id, OrientationConstraint::inputCategory, inputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::getAttributesByCategory(OrientationConstraint::id, OrientationConstraint::outputCategory, outputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	unsigned int inputCount = inputs.length();
	unsigned int outputCount = outputs.length();

	for (unsigned int i = 0; i < inputCount; i++)
	{

		for (unsigned int j = 0; j < outputCount; j++)
		{

			CHECK_MSTATUS(OrientationConstraint::attributeAffects(inputs[i], outputs[j]));

		}

	}

	return MS::kSuccess;

};
