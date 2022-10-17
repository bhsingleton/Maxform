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
	
	this->transform = MMatrix::identity; 
	this->preTranslate = MVector::zero; 
	this->preRotate = MQuaternion::identity;
	this->preScale = MVector::one;
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

		return this->preRotate;

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
		quat = Maxformations::createRotationMatrix(this->transform);

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
		
		return this->transform;
		
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

		return Maxformations::createRotationMatrix(this->transform);

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

		return Maxformations::createScaleMatrix(this->transform);

	}
	else
	{

		return MPxTransformationMatrix::asScaleMatrix();

	}

};


MTransformationMatrix Matrix3::asTransformationMatrix() const
/**
Returns scale matrix.
The scale matrix takes points from object space to the space immediately following scale and shear transformations.

@return: The scale matrix.
*/
{

	if (this->overrideEnabled)
	{

		return MTransformationMatrix(this->transform);

	}
	else
	{

		return MPxTransformationMatrix::asTransformationMatrix();

	}

};


void Matrix3::updatePreTranslation(const MVector& preTranslate)
/**
Updates the internal pre-rotation quaternion.

@param matrix: The new pre-rotation quaternion.
@return: Void.
*/
{

	this->preTranslate = preTranslate;

};


void Matrix3::updatePreRotation(const MQuaternion& preRotate)
/**
Updates the internal pre-rotation quaternion.

@param matrix: The new pre-rotation quaternion.
@return: Void.
*/
{

	this->preRotate = preRotate;

};


void Matrix3::updatePreScale(const MVector& preScale)
/**
Updates the internal pre-rotation quaternion.

@param matrix: The new pre-rotation quaternion.
@return: Void.
*/
{

	this->preScale = preScale;

};


void Matrix3::updateTransform(const MMatrix& matrix)
/**
Updates the internal transformation matrix.

@param matrix: The new transformation matrix.
@return: Void.
*/
{

	this->transform = matrix;
	this->overrideEnabled = !this->transform.isEquivalent(MMatrix::identity);

};