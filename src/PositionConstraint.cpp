//
// File: PositionConstraint.cpp
//
// Dependency Graph Node: positionConstraint
//
// Author: Ben Singleton
//

#include "PositionConstraint.h"


MObject PositionConstraint::relative;
MObject PositionConstraint::offsetTranslate;
MObject PositionConstraint::offsetTranslateX;
MObject PositionConstraint::offsetTranslateY;
MObject PositionConstraint::offsetTranslateZ;
MObject PositionConstraint::localOrWorld;
MObject PositionConstraint::restTranslate;
MObject PositionConstraint::restTranslateX;
MObject PositionConstraint::restTranslateY;
MObject PositionConstraint::restTranslateZ;
MObject PositionConstraint::target;
MObject PositionConstraint::targetWeight;
MObject PositionConstraint::targetMatrix;
MObject PositionConstraint::targetParentMatrix;
MObject PositionConstraint::targetOffsetTranslate;
MObject PositionConstraint::targetOffsetTranslateX;
MObject PositionConstraint::targetOffsetTranslateY;
MObject PositionConstraint::targetOffsetTranslateZ;

MObject PositionConstraint::constraintTranslate;
MObject PositionConstraint::constraintTranslateX;
MObject PositionConstraint::constraintTranslateY;
MObject PositionConstraint::constraintTranslateZ;
MObject PositionConstraint::constraintMatrix;
MObject PositionConstraint::constraintInverseMatrix;
MObject PositionConstraint::constraintWorldMatrix;
MObject PositionConstraint::constraintWorldInverseMatrix;
MObject PositionConstraint::constraintParentInverseMatrix;

MString	PositionConstraint::inputCategory("Input");
MString	PositionConstraint::offsetCategory("Offset");
MString	PositionConstraint::restCategory("Rest");
MString	PositionConstraint::targetCategory("Target");
MString	PositionConstraint::outputCategory("Output");

MTypeId	PositionConstraint::id(0x0013b1d5);

PositionConstraint::PositionConstraint() {};
PositionConstraint::~PositionConstraint() {};


MStatus PositionConstraint::compute(const MPlug& plug, MDataBlock& data)
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

	if (fnAttribute.hasCategory(PositionConstraint::outputCategory))
	{

		// Get input data handles
		//
		MDataHandle relativeHandle = data.inputValue(PositionConstraint::relative, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle offsetTranslateHandle = data.inputValue(PositionConstraint::offsetTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localOrWorldHandle = data.inputValue(PositionConstraint::localOrWorld, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle enableRestPositionHandle = data.inputValue(PositionConstraint::enableRestPosition, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle restTranslateHandle = data.inputValue(PositionConstraint::restTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintParentInverseMatrixHandle = data.inputValue(PositionConstraint::constraintParentInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle targetArrayHandle = data.inputArrayValue(PositionConstraint::target, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		bool localOrWorld = localOrWorldHandle.asBool();
		bool relative = relativeHandle.asBool();
		MVector offsetTranslate = offsetTranslateHandle.asVector();
		MMatrix offsetMatrix = relative ? Maxformations::createPositionMatrix(offsetTranslate) : MMatrix::identity;

		MMatrix constraintParentInverseMatrix = constraintParentInverseMatrixHandle.asMatrix();
		MMatrix constraintParentMatrix = constraintParentInverseMatrix.inverse();

		bool restEnabled = enableRestPositionHandle.asBool();
		MVector restTranslate = restTranslateHandle.asVector();
		MMatrix restTranslateMatrix = Maxformations::createPositionMatrix(restTranslate);
		MMatrix restMatrix = restEnabled ? restTranslateMatrix : MMatrix::identity;
		MMatrix restWorldMatrix = restTranslateMatrix * constraintParentMatrix;

		// Collect target matrices
		//
		unsigned int targetCount = targetArrayHandle.elementCount();

		MFloatArray targetWeights = MFloatArray(targetCount);
		MMatrixArray targetMatrices = MMatrixArray(targetCount);
		MMatrixArray targetWorldMatrices = MMatrixArray(targetCount);

		MDataHandle targetHandle, targetWeightHandle, targetMatrixHandle, targetParentMatrixHandle, targetOffsetTranslateHandle;
		MVector targetOffsetTranslate;
		MMatrix targetMatrix, targetParentMatrix, targetOffsetMatrix;

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
			targetWeightHandle = targetHandle.child(PositionConstraint::targetWeight);
			targetMatrixHandle = targetHandle.child(PositionConstraint::targetMatrix);
			targetParentMatrixHandle = targetHandle.child(PositionConstraint::targetParentMatrix);
			targetOffsetTranslateHandle = targetHandle.child(PositionConstraint::targetOffsetTranslate);

			// Get target weight
			//
			targetWeights[i] = Maxformations::clamp(targetWeightHandle.asFloat(), 0.0f, 100.0f) / 100.0f;

			// Get target matrices
			//
			targetMatrix = targetMatrixHandle.asMatrix();
			targetParentMatrix = targetParentMatrixHandle.asMatrix();
			targetOffsetTranslate = targetOffsetTranslateHandle.asVector();
			targetOffsetMatrix = relative ? Maxformations::createPositionMatrix(targetOffsetTranslate) : MMatrix::identity;

			targetMatrices[i] = Maxformations::createPositionMatrix(targetOffsetMatrix * targetMatrix);
			targetWorldMatrices[i] = Maxformations::createPositionMatrix(targetMatrices[i] * targetParentMatrix);

		}

		// Calculate weighted constraint matrix
		//
		MMatrix positionMatrix, constraintMatrix, constraintWorldMatrix;

		if (localOrWorld)  // Local
		{

			positionMatrix = Maxformations::blendMatrices(restMatrix, targetMatrices, targetWeights);

			constraintMatrix = offsetMatrix * positionMatrix;
			constraintWorldMatrix = constraintMatrix * constraintParentMatrix;

		}
		else  // World
		{

			positionMatrix = Maxformations::blendMatrices(restWorldMatrix, targetWorldMatrices, targetWeights);

			constraintWorldMatrix = offsetMatrix * positionMatrix;
			constraintMatrix = constraintWorldMatrix * constraintParentInverseMatrix;

		}

		// Get output data handles
		//
		MDataHandle constraintTranslateXHandle = data.outputValue(PositionConstraint::constraintTranslateX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintTranslateYHandle = data.outputValue(PositionConstraint::constraintTranslateY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintTranslateZHandle = data.outputValue(PositionConstraint::constraintTranslateZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintMatrixHandle = data.outputValue(PositionConstraint::constraintMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintInverseMatrixHandle = data.outputValue(PositionConstraint::constraintInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldMatrixHandle = data.outputValue(PositionConstraint::constraintWorldMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldInverseMatrixHandle = data.outputValue(PositionConstraint::constraintWorldInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		MVector constraintTranslate = Maxformations::matrixToPosition(constraintMatrix);
		MDistance::Unit internalUnit = MDistance::internalUnit();

		constraintTranslateXHandle.setMDistance(MDistance(constraintTranslate.x, internalUnit));
		constraintTranslateYHandle.setMDistance(MDistance(constraintTranslate.y, internalUnit));
		constraintTranslateZHandle.setMDistance(MDistance(constraintTranslate.z, internalUnit));

		constraintTranslateXHandle.setClean();
		constraintTranslateYHandle.setClean();
		constraintTranslateZHandle.setClean();

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


const MObject PositionConstraint::targetAttribute() const
/**
Returns the target attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return PositionConstraint::target;

};


const MObject PositionConstraint::weightAttribute() const
/**
Returns the weight attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return PositionConstraint::targetWeight;

};


void* PositionConstraint::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: TransformConstraint
*/
{

	return new PositionConstraint();

};


MStatus PositionConstraint::initialize()
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
	PositionConstraint::localOrWorld = fnNumericAttr.create("localOrWorld", "low", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::inputCategory));

	// ".relative" attribute
	//
	PositionConstraint::relative = fnNumericAttr.create("relative", "rel", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::inputCategory));

	// ".offsetTranslateX" attribute
	//
	PositionConstraint::offsetTranslateX = fnUnitAttr.create("offsetTranslateX", "otx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::offsetCategory));

	// ".offsetTranslateY" attribute
	//
	PositionConstraint::offsetTranslateY = fnUnitAttr.create("offsetTranslateY", "oty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::offsetCategory));

	// ".offsetTranslateZ" attribute
	//
	PositionConstraint::offsetTranslateZ = fnUnitAttr.create("offsetTranslateZ", "otz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::offsetCategory));

	// ".offsetTranslate" attribute
	//
	PositionConstraint::offsetTranslate = fnNumericAttr.create("offsetTranslate", "ot", PositionConstraint::offsetTranslateX, PositionConstraint::offsetTranslateY, PositionConstraint::offsetTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::offsetCategory));

	// ".enableRestPosition" attribute
	//
	status = fnNumericAttr.setObject(PositionConstraint::enableRestPosition);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::restCategory));

	// ".restTranslateX" attribute
	//
	PositionConstraint::restTranslateX = fnUnitAttr.create("restTranslateX", "rtx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::restCategory));

	// ".restTranslateY" attribute
	//
	PositionConstraint::restTranslateY = fnUnitAttr.create("restTranslateY", "rty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::restCategory));

	// ".restTranslateZ" attribute
	//
	PositionConstraint::restTranslateZ = fnUnitAttr.create("restTranslateZ", "rtz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::restCategory));

	// ".restTranslate" attribute
	//
	PositionConstraint::restTranslate = fnNumericAttr.create("restTranslate", "rt", PositionConstraint::restTranslateX, PositionConstraint::restTranslateY, PositionConstraint::restTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::restCategory));

	// ".constraintParentInverseMatrix" attribute
	//
	PositionConstraint::constraintParentInverseMatrix = fnMatrixAttr.create("constraintParentInverseMatrix", "cpim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::inputCategory));

	// Target Attributes:
	// ".targetWeight" attribute
	//
	PositionConstraint::targetWeight = fnNumericAttr.create("targetWeight", "tw", MFnNumericData::kFloat, 50.0f, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0f));
	CHECK_MSTATUS(fnNumericAttr.setMax(100.0f));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::targetCategory));

	// ".targetMatrix" attribute
	//
	PositionConstraint::targetMatrix = fnMatrixAttr.create("targetMatrix", "tm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::targetCategory));

	// ".targetParentMatrix" attribute
	//
	PositionConstraint::targetParentMatrix = fnMatrixAttr.create("targetParentMatrix", "tpm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::targetCategory));

	// ".targetOffsetTranslateX" attribute
	//
	PositionConstraint::targetOffsetTranslateX = fnUnitAttr.create("targetOffsetTranslateX", "totx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::offsetCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::targetCategory));

	// ".targetOffsetTranslateY" attribute
	//
	PositionConstraint::targetOffsetTranslateY = fnUnitAttr.create("targetOffsetTranslateY", "toty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::offsetCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::targetCategory));

	// ".targetOffsetTranslateZ" attribute
	//
	PositionConstraint::targetOffsetTranslateZ = fnUnitAttr.create("targetOffsetTranslateZ", "totz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::offsetCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::targetCategory));

	// ".targetOffsetTranslate" attribute
	//
	PositionConstraint::targetOffsetTranslate = fnNumericAttr.create("targetOffsetTranslate", "tot", PositionConstraint::targetOffsetTranslateX, PositionConstraint::targetOffsetTranslateY, PositionConstraint::targetOffsetTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::offsetCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::targetCategory));

	// ".target" attribute
	//
	PositionConstraint::target = fnCompoundAttr.create("target", "tg", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionConstraint::targetWeight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionConstraint::targetMatrix));
	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionConstraint::targetParentMatrix));
	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionConstraint::targetOffsetTranslate));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(PositionConstraint::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(PositionConstraint::targetCategory));

	// Output attributes:
	// ".constraintTranslateX" attribute
	//
	PositionConstraint::constraintTranslateX = fnUnitAttr.create("constraintTranslateX", "ctx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::outputCategory));

	// ".constraintTranslateY" attribute
	//
	PositionConstraint::constraintTranslateY = fnUnitAttr.create("constraintTranslateY", "cty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::outputCategory));

	// ".constraintTranslateZ" attribute
	//
	PositionConstraint::constraintTranslateZ = fnUnitAttr.create("constraintTranslateZ", "ctz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionConstraint::outputCategory));

	// ".constraintTranslate" attribute
	//
	PositionConstraint::constraintTranslate = fnNumericAttr.create("constraintTranslate", "ct", PositionConstraint::constraintTranslateX, PositionConstraint::constraintTranslateY, PositionConstraint::constraintTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionConstraint::outputCategory));

	// ".constraintMatrix" attribute
	//
	PositionConstraint::constraintMatrix = fnMatrixAttr.create("constraintMatrix", "cm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::outputCategory));

	// ".constraintInverseMatrix" attribute
	//
	PositionConstraint::constraintInverseMatrix = fnMatrixAttr.create("constraintInverseMatrix", "cim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::outputCategory));

	// ".constraintWorldMatrix" attribute
	//
	PositionConstraint::constraintWorldMatrix = fnMatrixAttr.create("constraintWorldMatrix", "cwm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::outputCategory));

	// ".constraintWorldInverseMatrix" attribute
	//
	PositionConstraint::constraintWorldInverseMatrix = fnMatrixAttr.create("constraintWorldInverseMatrix", "cwim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionConstraint::outputCategory));

	// Add attributes
	//
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::localOrWorld));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::relative));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::offsetTranslate));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::restTranslate));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::target));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::constraintTranslate));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::constraintMatrix));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::constraintInverseMatrix));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::constraintWorldMatrix));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::constraintWorldInverseMatrix));
	CHECK_MSTATUS(PositionConstraint::addAttribute(PositionConstraint::constraintParentInverseMatrix));

	// Define attribute relationships
	//
	MObjectArray inputs, outputs;

	status = Maxformations::getAttributesByCategory(PositionConstraint::id, PositionConstraint::inputCategory, inputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::getAttributesByCategory(PositionConstraint::id, PositionConstraint::outputCategory, outputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	unsigned int inputCount = inputs.length();
	unsigned int outputCount = outputs.length();

	for (unsigned int i = 0; i < inputCount; i++)
	{

		for (unsigned int j = 0; j < outputCount; j++)
		{

			CHECK_MSTATUS(PositionConstraint::attributeAffects(inputs[i], outputs[j]));

		}

	}

	return MS::kSuccess;

};
