//
// File: ExposeTransform.cpp
//
// Dependency Graph Node: exposeTm
//
// Author: Benjamin H. Singleton
//

#include "Maxform.h"

MObject	Maxform::transform;

MObject	Maxform::translationPart;
MObject	Maxform::rotationPart;
MObject	Maxform::scalePart;

MString	Maxform::matrixCategory("Matrix");
MString	Maxform::matrixPartsCategory("MatrixParts");
MString	Maxform::worldMatrixCategory("WorldMatrix");
MString	Maxform::parentMatrixCategory("ParentMatrix");

MString	Maxform::classification("drawdb/geometry/transform/maxform");
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
	bool isWorldMatrix = fnAttribute.hasCategory(Maxform::worldMatrixCategory);
	bool isMatrixPart = fnAttribute.hasCategory(Maxform::matrixPartsCategory);
	
	if (isMatrix || isWorldMatrix)
	{
		
		// Compute local transformation matrix
		//
		MPxTransformationMatrix* xform = this->transformationMatrixPtr();

		status = this->computeLocalTransformation(xform, data);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		// Call parent function
		//
		return MPxTransform::compute(plug, data);

	}
	else if (isMatrixPart)
	{

		// Get transformation matrix
		//
		MDataHandle matrixHandle = data.inputValue(Maxform::matrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTransformationMatrix transform = Maxformations::getTransformData(matrixHandle.data());
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

	// Call parent function
	//
	status = MPxTransform::computeLocalTransformation(xform, data);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Check if `Matrix3` was passed
	//
	Matrix3* matrix3 = dynamic_cast<Matrix3*>(xform);

	if (matrix3 != nullptr)
	{

		// Update matrix3
		//
		MDataHandle transformHandle = data.inputValue(Maxform::transform, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MObject transformData = transformHandle.data();
		MTransformationMatrix transform = Maxformations::getTransformData(transformData);

		matrix3->setTransform(transform);

		return MS::kSuccess;

	}
	else
	{

		return MS::kFailure;

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


void Maxform::getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const
/**
Provide node-specific setup info for the Cached Playback system.

@param evaluationNode: This node's evaluation node, contains animated plug information.
@param disablingInfo: Information about why the node disables Cached Playback to be reported to the user.
@param cacheSetupInfo: Preferences and requirements this node has for Cached Playback.
@param monitoredAttributes: Attributes impacting the behavior of this method that will be monitored for change.
@return: void.
*/
{

	// Call parent function
	//
	MPxTransform::getCacheSetup(evaluationNode, disablingInfo, cacheSetupInfo, monitoredAttributes);
	assert(!disablingInfo.getCacheDisabled());

	// Update caching preference
	//
	cacheSetupInfo.setPreference(MNodeCacheSetupInfo::kWantToCacheByDefault, true);

	// Append attributes for monitoring
	//
	monitoredAttributes.append(Maxform::transform);

};


MStatus Maxform::validateAndSetValue(const MPlug& plug, const MDataHandle& handle)
/**
When a plug's value is set, and the plug is on a default transform attribute, or has been flagged by the `mustCallValidateAndSet` method, then this method will be called.
The purpose of `validateAndSetValue` is to enforce limits, constraints, or plug value locking.
If the plug passed into this method is not an attribute related to the derived class, the derived class should call the validateAndSetValue method of its parent class in order to allow the base classes to handle their attributes.
If any adjustments or corrections are required, they are placed in the data block and if the context is normal, into the cached transformation matrix.
Values on the data block are in transform space.
Formerly the context was passed in; now the context will already be set as the current one for evaluation so it isn't necessary. To retrieve the current evaluation context, call MDGContext::current.
If you have specialty code that calls this method directly you'll have to ensure the current context is set using MDGContextGuard or MDGContext::makeCurrent.

@param plug: The plug that is to be set.
@param handle: The input handle of the data.
@return: Success.
*/
{

	MStatus status;

	// Inspect plug attribute
	//
	if (plug == Maxform::transform)
	{
		
		// Get data block from handle
		// If this fails then get the data block from the current context
		// 
		MDataBlock data = handle.datablock(&status);

		if (!status) 
		{

			data = this->forceCache();

		}

		// Recompute local transformation matrix
		//
		MPxTransformationMatrix* xform = this->transformationMatrixPtr();

		status = this->computeLocalTransformation(xform, data);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Copy transform data to output handle
		//
		MDataHandle outputHandle = data.outputValue(plug, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		status = outputHandle.copy(handle);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		outputHandle.setClean();
		data.setClean(plug);

		// Mark matrix as dirty to force DAG update
		//
		status = this->dirtyMatrix();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else
	{

		// Call parent function
		//
		return MPxTransform::validateAndSetValue(plug, handle);

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

	// Define attribute validations
	//
	Maxform::mustCallValidateAndSet(Maxform::transform);

	return status;

};