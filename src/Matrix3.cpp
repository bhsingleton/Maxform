//
// File: Matrix3.cpp
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "Matrix3.h"

MTypeId		Matrix3::id(0x0013b1cd);


Matrix3::Matrix3()
{ 
	
	this->transformValue = MMatrix::identity; 
	this->preTranslateValue = MVector::zero; 
	this->preRotateValue = MQuaternion::identity;
	this->preScaleValue = MVector::one;
	this->overrideEnabled = false;

};


Matrix3::~Matrix3() {};


MPxTransformationMatrix* Matrix3::creator()
/**
A method to use when registering a custom transform that uses a default MPxTransformationMatrix.
The caller is responsible for deleting the memory created by this method.

@return: The new MPxTransformationMatrix.
*/
{

	return new Matrix3();

};


MQuaternion Matrix3::preRotation() const
/**
Returns the rotate orientation for the transformation matrix as a quaternion.
The rotate orientation orients the local rotation space.

@param space: The space to use.
@param status: The return status.
@return: The rotate orientation.
*/
{

	if (this->overrideEnabled)
	{

		return this->preRotateValue;

	}
	else
	{

		return MPxTransformationMatrix::preRotation();

	}

};


MQuaternion Matrix3::rotateOrientation(MSpace::Space space, MStatus* status) const
/**
Returns the rotate orientation for the transformation matrix as a quaternion.
The rotate orientation orients the local rotation space.

@param space: The space to use.
@param status: The return status.
@return: The rotate orientation.
*/
{
	
	if (this->overrideEnabled && space == MSpace::kTransform)
	{

		MQuaternion quat;
		quat = Maxformations::createRotationMatrix(this->transformValue);

		return quat;

	}
	else
	{

		return MPxTransformationMatrix::rotateOrientation(space, status);
		
	}

};


MMatrix Matrix3::asMatrix() const
/**
Returns the four by four matrix that describes this transformation.

@return: Four by four matrix.
*/
{
	
	if (this->overrideEnabled)
	{
		
		return this->transformValue;
		
	}
	else
	{
		
		return MPxTransformationMatrix::asMatrix();
		
	}
	
};


MMatrix Matrix3::asRotateMatrix() const
/**
Returns the rotate section of the transformation matrix.

@return: The rotate matrix.
*/
{

	if (this->overrideEnabled)
	{

		return Maxformations::createRotationMatrix(this->transformValue);

	}
	else
	{

		return MPxTransformationMatrix::asRotateMatrix();

	}

};


MMatrix Matrix3::asScaleMatrix() const
/**
Returns scale matrix.
The scale matrix takes points from object space to the space immediately following scale and shear transformations.

@return: The scale matrix.
*/
{

	if (this->overrideEnabled)
	{

		return Maxformations::createScaleMatrix(this->transformValue);

	}
	else
	{

		return MPxTransformationMatrix::asScaleMatrix();

	}

};


void Matrix3::setPreTranslation(const MVector& preTranslate)
/**
Updates the internal pre-rotation quaternion.

@param matrix: The new pre-rotation quaternion.
@return: Void.
*/
{

	this->preTranslateValue = preTranslate;

};


void Matrix3::setPreRotation(const MQuaternion& preRotate)
/**
Updates the internal pre-rotation quaternion.

@param matrix: The new pre-rotation quaternion.
@return: Void.
*/
{

	this->preRotateValue = preRotate;

};


void Matrix3::setPreScale(const MVector& preScale)
/**
Updates the internal pre-rotation quaternion.

@param matrix: The new pre-rotation quaternion.
@return: Void.
*/
{

	this->preScaleValue = preScale;

};


void Matrix3::setTransform(const MMatrix& matrix)
/**
Updates the internal transformation matrix.

@param matrix: The new transformation matrix.
@return: Void.
*/
{

	this->transformValue = matrix;

};


void Matrix3::enableOverride()
/**
Updates the internal transformation matrix.

@param matrix: The new transformation matrix.
@return: Void.
*/
{

	this->overrideEnabled = true;

};


void Matrix3::disableOverride()
/**
Updates the internal transformation matrix.

@param matrix: The new transformation matrix.
@return: Void.
*/
{

	this->overrideEnabled = false;

};