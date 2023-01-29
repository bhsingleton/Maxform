//
// File: ExposeTransform.cpp
//
// Dependency Graph Node: exposeTm
//
// Author: Benjamin H. Singleton
//

#include "Maxform.h"

MObject	Maxform::preRotate;
MObject	Maxform::preRotateX;
MObject	Maxform::preRotateY;
MObject	Maxform::preRotateZ;
MObject	Maxform::transform;

MObject	Maxform::translationPart;
MObject	Maxform::rotationPart;
MObject	Maxform::scalePart;

MString	Maxform::preRotateCategory("PreRotate");
MString	Maxform::matrixCategory("Matrix");
MString	Maxform::matrixPartsCategory("MatrixParts");
MString	Maxform::worldMatrixCategory("WorldMatrix");
MString	Maxform::parentMatrixCategory("ParentMatrix");


MString	Maxform::classification("drawdb/geometry/transform");
MTypeId	Maxform::id(0x0013b1cc);


Maxform::Maxform() {};
Maxform::~Maxform() {};


MStatus Maxform::compute(const MPlug& plug, MDataBlock& data) 
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

	// Evaluate requested plug
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	bool isMatrix = fnAttribute.hasCategory(Maxform::matrixCategory);
	bool isMatrixPart = fnAttribute.hasCategory(Maxform::matrixPartsCategory);
	
	if (isMatrix)
	{

		// Compute local transformation matrix
		//
		MPxTransformationMatrix* transform = this->transformationMatrixPtr();

		status = this->computeLocalTransformation(transform, data);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		// Call parent function
		//
		return MPxTransform::compute(plug, data);

	}
	else if (isMatrixPart)
	{

		// Get transform value
		//
		MDataHandle transformHandle = data.inputValue(Maxform::transform, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTransformationMatrix transform = Maxformations::getTransformData(transformHandle.data());
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update data handles
		//
		MDataHandle translationPartHandle = data.outputValue(Maxform::translationPart, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rotationPartHandle = data.outputValue(Maxform::rotationPart, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle scalePartHandle = data.outputValue(Maxform::scalePart, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		translationPartHandle.setMMatrix(Maxformations::createPositionMatrix(transform.asMatrix()));  // Why is there no `asTranslateMatrix` function?
		translationPartHandle.setClean();

		rotationPartHandle.setMMatrix(transform.asRotateMatrix());
		rotationPartHandle.setClean();

		scalePartHandle.setMMatrix(transform.asScaleMatrix());
		scalePartHandle.setClean();

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


MStatus Maxform::computeLocalTransformation(MPxTransformationMatrix* xform, MDataBlock& data)
/**
This method computes the transformation matrix for a passed data block and places the output into a passed transformation matrix.
The caller needs to allocate space for the passed transformation matrix.

@param xform: The transformation matrix to fill with computed values.
@param data: The data block used for the transformation matrix computation.
@return: Status code.
*/
{

	MStatus status;

	// Try and cast pointer to `Matrix3` pointer
	//
	Matrix3* matrix3 = dynamic_cast<Matrix3*>(xform);

	if (matrix3 == nullptr)
	{

		MGlobal::displayWarning("Unable to cast MPxTransformationMatrix* to Matrix3*");
		return MPxTransform::computeLocalTransformation(xform, data);

	}

	// Check if matrix3 has been enabled
	//
	if (this->matrix3Enabled)
	{

		// Ensure matrix3 is enabled
		//
		matrix3->enable();

		// Get input data handles
		//
		MDataHandle rotateOrderHandle = data.inputValue(Maxform::rotateOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preRotateHandle = data.inputValue(Maxform::preRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle transformHandle = data.inputValue(Maxform::transform, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Compose pre-rotation
		//
		unsigned int rotateOrder = rotateOrderHandle.asShort();
		MVector radians = preRotateHandle.asVector();

		MQuaternion preRotation = MEulerRotation(radians, MEulerRotation::RotationOrder(rotateOrder)).asQuaternion();

		// Update matrix3 values
		//
		MTransformationMatrix transform = Maxformations::getTransformData(transformHandle.data());

		matrix3->setPreRotation(preRotation);
		matrix3->setTransform(transform);

		return MS::kSuccess;

	}
	else
	{

		// Disable matrix3
		//
		matrix3->disable();

		// Call parent function
		//
		return MPxTransform::computeLocalTransformation(xform, data);

	}

};


bool Maxform::treatAsTransform() const
/**
Maya's base transform node type is treated differently from node types which are derived from it.
For example, the 'viewFit' command does not include transform nodes in its calculations but does include pointConstraint nodes which are derived from the transform node.
By default, all custom transform node types are treated the same as Maya's base transform node type. Using the same example, by default a custom transform node will be excluded from the 'viewFit' command's calculations.
This method allows that default behaviour to be changed. By overriding this method to return false, a custom node can turn off the special treatment accorded to transform nodes and instead have itself treated the same as Maya treats derived transform nodes.

@return: True to be treated like a base transform node, false to be treated as a derived transform node.
*/
{

	return true;

};


MStatus Maxform::validateAndSetValue(const MPlug& plug, const MDataHandle& handle)
/**
When a plug's value is set, and the plug is on a default transform attribute, or has been flagged by the mustCallValidateAndSet() method, then this method will be called.
The purpose of validateAndSetValue() is to enforce limits, constraints, or plug value locking.
If the plug passed into this method is not an attribute related to the derived class, the derived class should call the validateAndSetValue method of its parent class in order to allow the base classes to handle their attributes.
If any adjustments or corrections are required, they are placed in the data block and if the context is normal, into the cached transformation matrix.
Values on the data block are in transform space.
Formerly the context was passed in; now the context will already be set as the current one for evaluation so it isn't necessary. To retrieve the current evaluation context, call MDGContext::current.
If you have specialty code that calls this method directly you'll have to ensure the current context is set using MDGContextGuard or MDGContext::makeCurrent.

@param plug: The plug that is to be set.
@param handle: The inputValue handle of the data.
@return: Success.
*/
{

	MStatus status;

	// Inspect plug attribute
	//
	if (plug == Maxform::transform)
	{

		// Recompute local transformation matrix
		//
		status = this->updateMatrixAttrs();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataBlock data = this->forceCache();
		MPxTransformationMatrix* transform = this->transformationMatrixPtr();

		status = this->computeLocalTransformation(transform, data);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Mark matrix as dirty 
		//
		status = this->dirtyMatrix();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else
	{

		return MPxTransform::validateAndSetValue(plug, handle);

	}

};


MStatus Maxform::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (attribute == Maxform::transform && !asSrc)
	{

		this->matrix3Enabled = true;

	}
	else;

	return MPxNode::connectionMade(plug, otherPlug, asSrc);

};


MStatus Maxform::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (attribute == Maxform::transform && !asSrc)
	{

		this->matrix3Enabled = false;

	}
	else;

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


Matrix3* Maxform::matrix3Ptr()
/**
This function returns a pointer to the cached Matrix3 for the current context.
The result transformation matrix is not guaranteed to be update-to-date. 
To get the valid transformation matrix, call updateMatrixAttrs() before this method!

@return: The current Matrix3.
*/
{

	MPxTransformationMatrix* transform = this->transformationMatrixPtr();

	if (transform != nullptr)
	{

		return static_cast<Matrix3*>(transform);

	}
	else
	{

		return nullptr;

	}

};


void* Maxform::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: Maxform
*/
{

	return new Maxform();

};


MPxTransformationMatrix* Maxform::createTransformationMatrix()
/**
This method returns a new transformation matrix.
The function that calls this method is responsible for deleting the transformation matrix.
If a derived MPxTransformationMatrix is used as the transformation matrix for the node, then this method must be overridden.

@return: A pointer to the new transformation matrix.
*/
{

	return new Matrix3();

};


MStatus Maxform::initialize()
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
	MFnUnitAttribute fnUnitAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnTypedAttribute fnTypedAttr;
	MFnEnumAttribute fnEnumAttr;

	// Input attributes:
	// ".preRotateX" attribute
	//
	Maxform::preRotateX = fnUnitAttr.create("preRotateX", "prx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(Maxform::preRotateCategory));

	// ".preRotateY" attribute
	//
	Maxform::preRotateY = fnUnitAttr.create("preRotateY", "pry", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(Maxform::preRotateCategory));

	// ".preRotateZ" attribute
	//
	Maxform::preRotateZ = fnUnitAttr.create("preRotateZ", "prz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(Maxform::preRotateCategory));

	// ".preRotate" attribute
	//
	Maxform::preRotate = fnNumericAttr.create("preRotate", "pr", Maxform::preRotateX, Maxform::preRotateY, Maxform::preRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preRotateCategory));

	// ".transform" attribute
	//
	Maxform::transform = fnTypedAttr.create("transform", "tf", MFnData::kMatrix, Maxformations::createMatrixData(MTransformationMatrix::identity), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.setAffectsWorldSpace(true));

	// Output attributes:
	// ".translationPart" attribute
	//
	Maxform::translationPart = fnMatrixAttr.create("translationPart", "tprt", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(Maxform::matrixPartsCategory));

	// ".rotationPart" attribute
	//
	Maxform::rotationPart = fnMatrixAttr.create("rotationPart", "rprt", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(Maxform::matrixPartsCategory));

	// ".scalePart" attribute
	//
	Maxform::scalePart = fnMatrixAttr.create("scalePart", "sprt", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(Maxform::matrixPartsCategory));

	// ".matrix" attribute
	//
	status = fnTypedAttr.setObject(Maxform::matrix);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(Maxform::matrixCategory));

	// ".inverseMatrix" attribute
	//
	status = fnTypedAttr.setObject(Maxform::inverseMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(Maxform::matrixCategory));

	// ".parentMatrix" attribute
	//
	status = fnTypedAttr.setObject(Maxform::parentMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(Maxform::parentMatrixCategory));

	// ".parentInverseMatrix" attribute
	//
	status = fnTypedAttr.setObject(Maxform::parentInverseMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(Maxform::parentMatrixCategory));

	// ".worldMatrix" attribute
	//
	status = fnTypedAttr.setObject(Maxform::worldMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(Maxform::worldMatrixCategory));

	// ".worldInverseMatrix" attribute
	//
	status = fnTypedAttr.setObject(Maxform::worldInverseMatrix);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(Maxform::worldMatrixCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(Maxform::addAttribute(Maxform::preRotate));
	CHECK_MSTATUS(Maxform::addAttribute(Maxform::transform));

	CHECK_MSTATUS(Maxform::addAttribute(Maxform::translationPart));
	CHECK_MSTATUS(Maxform::addAttribute(Maxform::rotationPart));
	CHECK_MSTATUS(Maxform::addAttribute(Maxform::scalePart));

	// Define attribute relationships
	//
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::translationPart));
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::rotationPart));
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::scalePart));
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::matrix));
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::inverseMatrix));
	//CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::parentMatrix));
	//CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::parentInverseMatrix));
	//CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::worldMatrix));
	//CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::worldInverseMatrix));

	// Define attribute validations
	//
	//Maxform::mustCallValidateAndSet(Maxform::preRotate);
	Maxform::mustCallValidateAndSet(Maxform::transform);

	return status;

};