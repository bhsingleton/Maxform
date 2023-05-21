//
// File: AttachmentConstraintNode.cpp
//
// Dependency Graph Node: attachmentConstraint
//
// Author: Ben Singleton
//

#include "AttachmentConstraint.h"


MObject AttachmentConstraint::relative;
MObject AttachmentConstraint::restTranslate;
MObject AttachmentConstraint::restTranslateX;
MObject AttachmentConstraint::restTranslateY;
MObject AttachmentConstraint::restTranslateZ;
MObject AttachmentConstraint::restRotate;
MObject AttachmentConstraint::restRotateX;
MObject AttachmentConstraint::restRotateY;
MObject AttachmentConstraint::restRotateZ;
MObject AttachmentConstraint::offsetTranslate;
MObject AttachmentConstraint::offsetTranslateX;
MObject AttachmentConstraint::offsetTranslateY;
MObject AttachmentConstraint::offsetTranslateZ;
MObject AttachmentConstraint::offsetRotate;
MObject AttachmentConstraint::offsetRotateX;
MObject AttachmentConstraint::offsetRotateY;
MObject AttachmentConstraint::offsetRotateZ;
MObject AttachmentConstraint::target;
MObject AttachmentConstraint::targetWeight;
MObject AttachmentConstraint::targetFace;
MObject AttachmentConstraint::targetCoord;
MObject AttachmentConstraint::targetCoordX;
MObject AttachmentConstraint::targetCoordY;
MObject AttachmentConstraint::targetMesh;

MObject AttachmentConstraint::constraintTranslate;
MObject AttachmentConstraint::constraintTranslateX;
MObject AttachmentConstraint::constraintTranslateY;
MObject AttachmentConstraint::constraintTranslateZ;
MObject AttachmentConstraint::constraintRotate;
MObject AttachmentConstraint::constraintRotateX;
MObject AttachmentConstraint::constraintRotateY;
MObject AttachmentConstraint::constraintRotateZ;
MObject AttachmentConstraint::constraintRotateOrder;
MObject AttachmentConstraint::constraintMatrix;
MObject AttachmentConstraint::constraintInverseMatrix;
MObject AttachmentConstraint::constraintWorldMatrix;
MObject AttachmentConstraint::constraintWorldInverseMatrix;
MObject AttachmentConstraint::constraintParentInverseMatrix;

MString	AttachmentConstraint::inputCategory("Input");
MString	AttachmentConstraint::restCategory("Rest");
MString	AttachmentConstraint::offsetCategory("Offset");
MString	AttachmentConstraint::targetCategory("Target");
MString	AttachmentConstraint::outputCategory("Output");

MTypeId AttachmentConstraint::id(0x0013b1c9);


AttachmentConstraint::AttachmentConstraint() {}
AttachmentConstraint::~AttachmentConstraint() {};


MStatus AttachmentConstraint::compute(const MPlug& plug, MDataBlock& data)
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

	if (fnAttribute.hasCategory(AttachmentConstraint::outputCategory))
	{

		// Get input data handles
		//
		MDataHandle relativeHandle = data.inputValue(AttachmentConstraint::relative, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle offsetTranslateHandle = data.inputValue(AttachmentConstraint::offsetTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle offsetRotateHandle = data.inputValue(AttachmentConstraint::offsetRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle enableRestPositionHandle = data.inputValue(AttachmentConstraint::enableRestPosition, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle restTranslateHandle = data.inputValue(AttachmentConstraint::restTranslate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle restRotateHandle = data.inputValue(AttachmentConstraint::restRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintParentInverseMatrixHandle = data.inputValue(AttachmentConstraint::constraintParentInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateOrderHandle = data.inputValue(AttachmentConstraint::constraintRotateOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle targetArrayHandle = data.inputArrayValue(AttachmentConstraint::target, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		bool relative = relativeHandle.asBool();
		MVector offsetRotate = offsetRotateHandle.asVector();
		MMatrix offsetMatrix = relative ? Maxformations::createRotationMatrix(offsetRotate) : MMatrix::identity;

		MEulerRotation::RotationOrder constraintRotateOrder = MEulerRotation::RotationOrder(constraintRotateOrderHandle.asShort());
		MMatrix constraintParentInverseMatrix = constraintParentInverseMatrixHandle.asMatrix();
		MMatrix constraintParentMatrix = constraintParentInverseMatrix.inverse();

		bool restEnabled = enableRestPositionHandle.asBool();
		MVector restTranslate = restTranslateHandle.asVector();
		MMatrix restTranslateMatrix = Maxformations::createPositionMatrix(restTranslate);
		MVector restRotate = restRotateHandle.asVector();
		MMatrix restRotateMatrix = Maxformations::createRotationMatrix(restRotate, constraintRotateOrder);
		MMatrix restMatrix = restEnabled ? (restRotateMatrix * restTranslateMatrix) : MMatrix::identity;
		MMatrix restWorldMatrix = restMatrix * constraintParentMatrix;

		// Collect target matrices
		//
		unsigned int targetCount = targetArrayHandle.elementCount();
		MFloatArray targetWeights = MFloatArray(targetCount);
		MMatrixArray targetMatrices = MMatrixArray(targetCount);

		MDataHandle  targetHandle, targetWeightHandle, targetFaceHandle, targetCoordHandle, targetCoordXHandle, targetCoordYHandle, targetMeshHandle;

		MObject targetMesh;
		unsigned int targetFace;
		MVector targetCoord, targetTangent, targetBinormal;
		MPoint targetPosition;
		float targetCoordX, targetCoordY;
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
			targetWeightHandle = targetHandle.child(AttachmentConstraint::targetWeight);
			targetFaceHandle = targetHandle.child(AttachmentConstraint::targetFace);
			targetCoordHandle = targetHandle.child(AttachmentConstraint::targetCoord);
			targetCoordXHandle = targetCoordHandle.child(AttachmentConstraint::targetCoordX);
			targetCoordYHandle = targetCoordHandle.child(AttachmentConstraint::targetCoordY);
			targetMeshHandle = targetHandle.child(AttachmentConstraint::targetMesh);

			// Get weight value
			//
			targetWeights[i] = Maxformations::clamp(targetWeightHandle.asFloat(), 0.0f, 100.0f) / 100.0;

			// Get curve parameter
			//
			targetMesh = targetMeshHandle.asMesh();
			targetFace = targetFaceHandle.asLong();
			targetCoordX = targetCoordXHandle.asFloat();
			targetCoordY = targetCoordYHandle.asFloat();
			targetCoord = MVector(targetCoordX, targetCoordY, 1.0f - (targetCoordX + targetCoordY));

			// Compose transform matrix
			//
			status = Maxformations::composeMatrix(targetMesh, targetFace, targetCoord, targetMatrix);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			targetMatrices[i] = MMatrix(targetMatrix);

		}

		// Calculate weighted constraint matrix
		//
		MMatrix attachmentMatrix = Maxformations::blendMatrices(restWorldMatrix, targetMatrices, targetWeights);

		MMatrix constraintWorldMatrix = offsetMatrix * attachmentMatrix;
		MMatrix constraintMatrix = constraintWorldMatrix * constraintParentInverseMatrix;

		// Get output data handles
		//
		MDataHandle constraintTranslateXHandle = data.outputValue(AttachmentConstraint::constraintTranslateX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintTranslateYHandle = data.outputValue(AttachmentConstraint::constraintTranslateY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintTranslateZHandle = data.outputValue(AttachmentConstraint::constraintTranslateZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateXHandle = data.outputValue(AttachmentConstraint::constraintRotateX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateYHandle = data.outputValue(AttachmentConstraint::constraintRotateY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintRotateZHandle = data.outputValue(AttachmentConstraint::constraintRotateZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintMatrixHandle = data.outputValue(AttachmentConstraint::constraintMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintInverseMatrixHandle = data.outputValue(AttachmentConstraint::constraintInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldMatrixHandle = data.outputValue(AttachmentConstraint::constraintWorldMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle constraintWorldInverseMatrixHandle = data.outputValue(AttachmentConstraint::constraintWorldInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update translation constraints
		//
		MVector constraintTranslate = Maxformations::matrixToPosition(constraintMatrix);
		MDistance::Unit distanceUnit = MDistance::internalUnit();

		constraintTranslateXHandle.setMDistance(MDistance(constraintTranslate.x, distanceUnit));
		constraintTranslateYHandle.setMDistance(MDistance(constraintTranslate.y, distanceUnit));
		constraintTranslateZHandle.setMDistance(MDistance(constraintTranslate.z, distanceUnit));

		constraintTranslateXHandle.setClean();
		constraintTranslateYHandle.setClean();
		constraintTranslateZHandle.setClean();

		// Update rotation constraints
		//
		MEulerRotation constraintRotate = Maxformations::matrixToEulerRotation(constraintMatrix, constraintRotateOrder);
		MAngle::Unit angleUnit = MAngle::internalUnit();

		constraintRotateXHandle.setMAngle(MAngle(constraintRotate.x, angleUnit));
		constraintRotateYHandle.setMAngle(MAngle(constraintRotate.y, angleUnit));
		constraintRotateZHandle.setMAngle(MAngle(constraintRotate.z, angleUnit));

		constraintRotateXHandle.setClean();
		constraintRotateYHandle.setClean();
		constraintRotateZHandle.setClean();

		// Update matrix constraints
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


const MObject AttachmentConstraint::targetAttribute() const
/**
Returns the target attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return AttachmentConstraint::target;

};


const MObject AttachmentConstraint::weightAttribute() const
/**
Returns the weight attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return AttachmentConstraint::targetWeight;

};


const MObject AttachmentConstraint::constraintRotateOrderAttribute() const
/**
Returns the rotate order attribute for the constraint.
Default implementation returns MObject::kNullObj.

@return: MObject
*/
{


	return AttachmentConstraint::constraintRotateOrder;

};


void* AttachmentConstraint::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: AttachmentConstraint
*/
{

	return new AttachmentConstraint();

}


MStatus AttachmentConstraint::initialize()
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

	// Input attributes:
	// ".relative" attribute
	//
	AttachmentConstraint::relative = fnNumericAttr.create("relative", "rel", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));

	// ".offsetTranslateX" attribute
	//
	AttachmentConstraint::offsetTranslateX = fnUnitAttr.create("offsetTranslateX", "otx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".offsetTranslateY" attribute
	//
	AttachmentConstraint::offsetTranslateY = fnUnitAttr.create("offsetTranslateY", "oty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".offsetTranslateZ" attribute
	//
	AttachmentConstraint::offsetTranslateZ = fnUnitAttr.create("offsetTranslateZ", "otz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".offsetTranslate" attribute
	//
	AttachmentConstraint::offsetTranslate = fnNumericAttr.create("offsetTranslate", "ot", AttachmentConstraint::offsetTranslateX, AttachmentConstraint::offsetTranslateY, AttachmentConstraint::offsetTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".offsetRotateX" attribute
	//
	AttachmentConstraint::offsetRotateX = fnUnitAttr.create("offsetRotateX", "orx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".offsetRotateY" attribute
	//
	AttachmentConstraint::offsetRotateY = fnUnitAttr.create("offsetRotateY", "ory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".offsetRotateZ" attribute
	//
	AttachmentConstraint::offsetRotateZ = fnUnitAttr.create("offsetRotateZ", "orz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".offsetRotate" attribute
	//
	AttachmentConstraint::offsetRotate = fnNumericAttr.create("offsetRotate", "or", AttachmentConstraint::offsetRotateX, AttachmentConstraint::offsetRotateY, AttachmentConstraint::offsetRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::offsetCategory));

	// ".enableRestPosition" attribute
	//
	status = fnNumericAttr.setObject(AttachmentConstraint::enableRestPosition);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restTranslateX" attribute
	//
	AttachmentConstraint::restTranslateX = fnUnitAttr.create("restTranslateX", "rtx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restTranslateY" attribute
	//
	AttachmentConstraint::restTranslateY = fnUnitAttr.create("restTranslateY", "rty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restTranslateZ" attribute
	//
	AttachmentConstraint::restTranslateZ = fnUnitAttr.create("restTranslateZ", "rtz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restTranslate" attribute
	//
	AttachmentConstraint::restTranslate = fnNumericAttr.create("restTranslate", "rt", AttachmentConstraint::restTranslateX, AttachmentConstraint::restTranslateY, AttachmentConstraint::restTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restRotateX" attribute
	//
	AttachmentConstraint::restRotateX = fnUnitAttr.create("restRotateX", "rrx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restRotateY" attribute
	//
	AttachmentConstraint::restRotateY = fnUnitAttr.create("restRotateY", "rry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restRotateZ" attribute
	//
	AttachmentConstraint::restRotateZ = fnUnitAttr.create("restRotateZ", "rrz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".restRotate" attribute
	//
	AttachmentConstraint::restRotate = fnNumericAttr.create("restRotate", "rr", AttachmentConstraint::restRotateX, AttachmentConstraint::restRotateY, AttachmentConstraint::restRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::restCategory));

	// ".constraintRotateOrder" attribute
	//
	AttachmentConstraint::constraintRotateOrder = fnEnumAttr.create("constraintRotateOrder", "cro", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 5));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(AttachmentConstraint::inputCategory));

	// ".constraintParentInverseMatrix" attribute
	//
	AttachmentConstraint::constraintParentInverseMatrix = fnMatrixAttr.create("constraintParentInverseMatrix", "cpim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.addToCategory(AttachmentConstraint::inputCategory));

	// Target Attributes:
	// Define ".targetWeight" attribute
	//
	AttachmentConstraint::targetWeight = fnNumericAttr.create("targetWeight", "tw", MFnNumericData::kFloat, 50.0f, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0f));
	CHECK_MSTATUS(fnNumericAttr.setMax(100.0f));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::targetCategory));

	// Define ".targetFace" attribute
	//
	AttachmentConstraint::targetFace = fnNumericAttr.create("targetFace", "tf", MFnNumericData::kLong, 0L, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0L));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::targetCategory));

	// Define ".targetCoordX" attribute
	//
	AttachmentConstraint::targetCoordX = fnNumericAttr.create("targetCoordX", "tcx", MFnNumericData::kFloat, 0.0f, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::targetCategory));

	// Define ".targetCoordY" attribute
	//
	AttachmentConstraint::targetCoordY = fnNumericAttr.create("targetCoordY", "tcy", MFnNumericData::kFloat, 0.0f, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::targetCategory));

	// Define ".targetCoord" attribute
	//
	AttachmentConstraint::targetCoord = fnNumericAttr.create("targetCoord", "tc", AttachmentConstraint::targetCoordX, AttachmentConstraint::targetCoordY, MObject::kNullObj, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::targetCategory));

	// ".targetMesh" attribute
	//
	AttachmentConstraint::targetMesh = fnTypedAttr.create("targetMesh", "tm", MFnData::kMesh, MObject::kNullObj, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(AttachmentConstraint::targetCategory));

	// Define ".target" attribute
	//
	AttachmentConstraint::target = fnCompoundAttr.create("target", "tg", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnCompoundAttr.addChild(AttachmentConstraint::targetWeight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(AttachmentConstraint::targetFace));
	CHECK_MSTATUS(fnCompoundAttr.addChild(AttachmentConstraint::targetCoord));
	CHECK_MSTATUS(fnCompoundAttr.addChild(AttachmentConstraint::targetMesh));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(AttachmentConstraint::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(AttachmentConstraint::targetCategory));

	// Output attributes:
	// Define ".constraintTranslateX" attribute
	//
	AttachmentConstraint::constraintTranslateX = fnUnitAttr.create("constraintTranslateX", "ctx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintTranslateY" attribute
	//
	AttachmentConstraint::constraintTranslateY = fnUnitAttr.create("constraintTranslateY", "cty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintTranslateZ" attribute
	//
	AttachmentConstraint::constraintTranslateZ = fnUnitAttr.create("constraintTranslateZ", "ctz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintTranslate" attribute
	//
	AttachmentConstraint::constraintTranslate = fnNumericAttr.create("constraintTranslate", "ct", AttachmentConstraint::constraintTranslateX, AttachmentConstraint::constraintTranslateY, AttachmentConstraint::constraintTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintRotateX" attribute
	//
	AttachmentConstraint::constraintRotateX = fnUnitAttr.create("constraintRotateX", "crx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintRotateY" attribute
	//
	AttachmentConstraint::constraintRotateY = fnUnitAttr.create("constraintRotateY", "cry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintRotateZ" attribute
	//
	AttachmentConstraint::constraintRotateZ = fnUnitAttr.create("constraintRotateZ", "crz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintRotate" attribute
	//
	AttachmentConstraint::constraintRotate = fnNumericAttr.create("constraintRotate", "cr", AttachmentConstraint::constraintRotateX, AttachmentConstraint::constraintRotateY, AttachmentConstraint::constraintRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintMatrix" attribute
	//
	AttachmentConstraint::constraintMatrix = fnMatrixAttr.create("constraintMatrix", "cm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintInverseMatrix" attribute
	//
	AttachmentConstraint::constraintInverseMatrix = fnMatrixAttr.create("constraintInverseMatrix", "cim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintWorldMatrix" attribute
	//
	AttachmentConstraint::constraintWorldMatrix = fnMatrixAttr.create("constraintWorldMatrix", "cwm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(AttachmentConstraint::outputCategory));

	// ".constraintWorldInverseMatrix" attribute
	//
	AttachmentConstraint::constraintWorldInverseMatrix = fnMatrixAttr.create("constraintWorldInverseMatrix", "cwim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(AttachmentConstraint::outputCategory));

	// Add attributes
	//
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::restTranslate));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::restRotate));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::relative));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::offsetTranslate));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::offsetRotate));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::target));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintTranslate));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintRotateOrder));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintRotate));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintMatrix));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintInverseMatrix));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintWorldMatrix));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintWorldInverseMatrix));
	CHECK_MSTATUS(AttachmentConstraint::addAttribute(AttachmentConstraint::constraintParentInverseMatrix));

	// Define attribute relationships
	//
	MObjectArray inputs, outputs;

	status = Maxformations::getAttributesByCategory(AttachmentConstraint::id, AttachmentConstraint::inputCategory, inputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::getAttributesByCategory(AttachmentConstraint::id, AttachmentConstraint::outputCategory, outputs);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	unsigned int inputCount = inputs.length();
	unsigned int outputCount = outputs.length();

	for (unsigned int i = 0; i < inputCount; i++)
	{

		for (unsigned int j = 0; j < outputCount; j++)
		{

			CHECK_MSTATUS(AttachmentConstraint::attributeAffects(inputs[i], outputs[j]));

		}

	}

	return status;

};