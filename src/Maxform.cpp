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

	bool isMatrixPart = fnAttribute.hasCategory(Maxform::matrixPartsCategory);
	
	if (isMatrixPart)
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

		translationPartHandle.setMMatrix(Maxformations::createPositionMatrix(transform.asMatrix()));  // Why is there no .asTranslateMatrix()?
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

	// Try and cast pointer to matrix3
	//
	Matrix3* matrix3 = dynamic_cast<Matrix3*>(xform);

	if (matrix3 == nullptr)
	{

		return MPxTransform::computeLocalTransformation(xform, data);

	}

	// Check if matrix3 has been enabled
	//
	if (matrix3->isEnabled())
	{

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
		MEulerRotation::RotationOrder rotationOrder = MEulerRotation::RotationOrder(rotateOrderHandle.asShort());
		MVector eulerAngles = preRotateHandle.asVector();

		MQuaternion preRotate = MEulerRotation(eulerAngles, rotationOrder).asQuaternion();

		// Update matrix3 values
		//
		MTransformationMatrix transform = Maxformations::getTransformData(transformHandle.data());

		matrix3->setPreRotation(preRotate);
		matrix3->setTransform(transform);

		return MS::kSuccess;

	}
	else
	{

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


MPxNode::SchedulingType Maxform::schedulingType() const
/**
When overridden this method controls the degree of parallelism supported by the node during threaded evaluation.
Defaults to SchedulingType::kDefaultScheduling.

@return: The scheduling type to be used for this node. 
*/
{

	return MPxNode::SchedulingType::kParallel;

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
	monitoredAttributes.append(Maxform::preRotate);
	monitoredAttributes.append(Maxform::preRotateX);
	monitoredAttributes.append(Maxform::preRotateY);
	monitoredAttributes.append(Maxform::preRotateZ);
	monitoredAttributes.append(Maxform::transform);

};


MStatus Maxform::preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode)
/**
Prepare a node's internal state for threaded evaluation.
During the evaluation graph execution each node gets a chance to reset its internal states just before being evaluated.
This code has to be thread safe, non-blocking and work only on data owned by the node.
The timing of this callback is at the discretion of evaluation graph dependencies and individual evaluators. This means, it should be used purely to prepare this node for evaluation and no particular order should be assumed.
This call will most likely happen from a worker thread.
When using Evaluation Caching or VP2 Custom Caching, preEvaluation() is called as part of the evaluation process. This function is not called as part of the cache restore process because no evaluation takes place in that case.

@param context: Context in which the evaluation is happening. This should be respected and only internal state information pertaining to it should be modified.
@param evaluationNode: Evaluation node which contains information about the dirty plugs that are about to be evaluated for the context. Should be only used to query information.
@return: Return status.
*/
{

	MStatus status;

	if (context.isNormal())
	{

		if (evaluationNode.dirtyPlugExists(Maxform::preRotate, &status) && status ||
			evaluationNode.dirtyPlugExists(Maxform::preRotateX, &status) && status ||
			evaluationNode.dirtyPlugExists(Maxform::preRotateY, &status) && status ||
			evaluationNode.dirtyPlugExists(Maxform::preRotateZ, &status) && status ||
			evaluationNode.dirtyPlugExists(Maxform::transform, &status) && status)
		{

			this->dirtyMatrix();

		}

	}

	return MPxTransform::preEvaluation(context, evaluationNode);

};


MStatus Maxform::validateAndSetValue(const MPlug& plug, const MDataHandle& handle)
/**
When a plug's value is set, and the plug is on a default transform attribute, or has been flagged by the mustCallValidateAndSet() method, then this method will be called.
The purpose of validateAndSetValue() is to enforce limits, constraints, or plug value locking.
If the plug passed into this method is not an attribute related to the derived class, the derived class should call the validateAndSetValue method of its parent class in order to allow the base classes to handle their attributes.
If any adjustments or corrections are required, they are placed in the data block and if the context is normal, into the cached transformation matrix. Values on the data block are in transform space.
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
		MDataBlock data = this->forceCache();
		MPxTransformationMatrix* xform = this->transformationMatrixPtr();

		status = this->computeLocalTransformation(xform, data);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Mark matrix as dirty 
		//
		status = this->dirtyMatrix();
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	return MPxTransform::validateAndSetValue(plug, handle);

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

		Matrix3* matrix3 = this->matrix3Ptr();

		if (matrix3 != nullptr)
		{

			matrix3->enable();

		}

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

		Matrix3* matrix3 = this->matrix3Ptr();

		if (matrix3 != nullptr)
		{

			matrix3->disable();

		}

	}
	else;

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


Matrix3* Maxform::matrix3Ptr()
/**
This function returns a pointer to the cached Matrix3 for the current context.

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


MObjectHandle Maxform::thisMObjectHandle()
/**
Returns an object handle for this instance.

@return: Object handle.
*/
{

	return MObjectHandle(this->thisMObject());

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
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::parentMatrix));
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::parentInverseMatrix));
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::worldMatrix));
	CHECK_MSTATUS(Maxform::attributeAffects(Maxform::transform, Maxform::worldInverseMatrix));

	// Define attribute validations
	//
	Maxform::mustCallValidateAndSet(Maxform::transform);

	return status;

};