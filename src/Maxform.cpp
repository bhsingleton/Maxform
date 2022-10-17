//
// File: ExposeTransform.cpp
//
// Dependency Graph Node: exposeTm
//
// Author: Benjamin H. Singleton
//

#include "Maxform.h"

MObject		Maxform::preTranslate;
MObject		Maxform::preTranslateX;
MObject		Maxform::preTranslateY;
MObject		Maxform::preTranslateZ;
MObject		Maxform::preRotate;
MObject		Maxform::preRotateX;
MObject		Maxform::preRotateY;
MObject		Maxform::preRotateZ;
MObject		Maxform::preRotateW;
MObject		Maxform::preScale;
MObject		Maxform::preScaleX;
MObject		Maxform::preScaleY;
MObject		Maxform::preScaleZ;
MObject		Maxform::transform;

MObject		Maxform::translationPart;
MObject		Maxform::rotationPart;
MObject		Maxform::scalePart;

MString		Maxform::matrixCategory("Matrix");
MString		Maxform::partsCategory("Parts");
MString		Maxform::preTranslateCategory("PreTranslate");
MString		Maxform::preRotateCategory("PreRotate");
MString		Maxform::preScaleCategory("PreScale");

MTypeId		Maxform::id(0x0013b1cc);
MString		Maxform::classification("drawdb/geometry/transform/maxform");


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
	bool isMatrixPart = fnAttribute.hasCategory(Maxform::partsCategory);
	
	if (isMatrix)
	{

		// Compute local transformation matrix
		//
		status = this->computeLocalTransformation(this->transformationMatrixPtr(), data);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else if (isMatrixPart)
	{

		// Get input data handles
		//
		MDataHandle transformHandle = data.inputValue(Maxform::transform, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get input values
		//
		MMatrix transform = transformHandle.asMatrix();

		// Compute transform components
		//
		MMatrix translationPart = Maxformations::createPositionMatrix(transform);
		MMatrix rotationPart = Maxformations::createRotationMatrix(transform);
		MMatrix scalePart = Maxformations::createScaleMatrix(transform);

		// Get output data handles
		//
		MDataHandle translationPartHandle = data.outputValue(Maxform::translationPart, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle rotationPartHandle = data.outputValue(Maxform::rotationPart, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle scalePartHandle = data.outputValue(Maxform::scalePart, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		translationPartHandle.setMMatrix(translationPart);
		translationPartHandle.setClean();

		rotationPartHandle.setMMatrix(rotationPart);
		rotationPartHandle.setClean();

		scalePartHandle.setMMatrix(scalePart);
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

	// Get input handles
	//
	MDataHandle transformHandle = data.inputValue(Maxform::transform, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MDataHandle preTranslateHandle = data.inputValue(Maxform::preTranslate, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MDataHandle preRotateHandle = data.inputValue(Maxform::preRotate, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MDataHandle preScaleHandle = data.inputValue(Maxform::preScale, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Get input values
	//
	MMatrix transform = transformHandle.asMatrix();
	MVector preTranslate = preTranslateHandle.asDouble3();
	MQuaternion preRotate = preRotateHandle.asDouble4();
	MVector preScale = preScaleHandle.asDouble3();

	// Update transformation matrix
	//
	Matrix3* matrix3 = dynamic_cast<Matrix3*>(xform);
	matrix3->setTransform(transform);
	matrix3->setPreTranslation(preTranslate);
	matrix3->setPreRotation(preRotate);
	matrix3->setPreScale(preScale);

	return MS::kSuccess;

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

			matrix3->enableOverride();

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

			matrix3->disableOverride();

		}

	}
	else;

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


void Maxform::getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const
{

	// Call parent function
	//
	MPxTransform::getCacheSetup(evaluationNode, disablingInfo, cacheSetupInfo, monitoredAttributes);
	assert(!disablingInfo.getCacheDisabled());

	// Update caching preference
	//
	cacheSetupInfo.setPreference(MNodeCacheSetupInfo::kWantToCacheByDefault, false);

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

	// Input attributes:
	// ".preTranslateX" attribute
	//
	Maxform::preTranslateX = fnUnitAttr.create("preTranslateX", "ptx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(Maxform::preTranslateCategory));

	// ".preTranslateY" attribute
	//
	Maxform::preTranslateY = fnUnitAttr.create("preTranslateY", "pty", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(Maxform::preTranslateCategory));

	// ".preTranslateZ" attribute
	//
	Maxform::preTranslateZ = fnUnitAttr.create("preTranslateZ", "ptz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(Maxform::preTranslateCategory));

	// ".preTranslate" attribute
	//
	Maxform::preTranslate = fnNumericAttr.create("preTranslate", "pt", Maxform::preTranslateX, Maxform::preTranslateY, Maxform::preTranslateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preTranslateCategory));

	// ".preRotateX" attribute
	//
	Maxform::preRotateX = fnNumericAttr.create("preRotateX", "prx", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preRotateCategory));

	// ".preRotateY" attribute
	//
	Maxform::preRotateY = fnNumericAttr.create("preRotateY", "pry", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preRotateCategory));

	// ".preRotateZ" attribute
	//
	Maxform::preRotateZ = fnNumericAttr.create("preRotateZ", "prz", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preRotateCategory));

	// ".preRotateW" attribute
	//
	Maxform::preRotateW = fnNumericAttr.create("preRotateW", "prw", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preRotateCategory));

	// ".preRotate" attribute
	//
	Maxform::preRotate = fnNumericAttr.create("preRotate", "pr", Maxform::preRotateX, Maxform::preRotateY, Maxform::preRotateZ, Maxform::preRotateW, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preRotateCategory));

	// ".preScaleX" attribute
	//
	Maxform::preScaleX = fnNumericAttr.create("preScaleX", "psx", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preScaleCategory));

	// ".preScaleY" attribute
	//
	Maxform::preScaleY = fnNumericAttr.create("preScaleY", "psy", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preScaleCategory));

	// ".preScaleZ" attribute
	//
	Maxform::preScaleZ = fnNumericAttr.create("preScaleZ", "psz", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preScaleCategory));

	// ".preScale" attribute
	//
	Maxform::preScale = fnNumericAttr.create("preScale", "ps", Maxform::preScaleX, Maxform::preScaleY, Maxform::preScaleZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(Maxform::preScaleCategory));

	// ".transform" attribute
	//
	Maxform::transform = fnMatrixAttr.create("transform", "tf", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setAffectsWorldSpace(true));

	// Output attributes:
	// ".translationPart" attribute
	//
	Maxform::translationPart = fnMatrixAttr.create("translationPart", "tprt", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(Maxform::partsCategory));

	// ".rotationPart" attribute
	//
	Maxform::rotationPart = fnMatrixAttr.create("rotationPart", "rprt", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(Maxform::partsCategory));

	// ".scalePart" attribute
	//
	Maxform::scalePart = fnMatrixAttr.create("scalePart", "sprt", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(Maxform::partsCategory));

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

	// Add attributes to node
	//
	CHECK_MSTATUS(Maxform::addAttribute(Maxform::preTranslate));
	CHECK_MSTATUS(Maxform::addAttribute(Maxform::preRotate));
	CHECK_MSTATUS(Maxform::addAttribute(Maxform::preScale));
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