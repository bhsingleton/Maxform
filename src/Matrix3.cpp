//
// File: Matrix3.cpp
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "Matrix3.h"

MTypeId	Matrix3::id(0x0013b1cd);


Matrix3::Matrix3()
/**
Constructor.
*/
{ 
	
	this->matrixValue = MTransformationMatrix::identity; 

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


MPxTransformationMatrix& Matrix3::operator=(const MPxTransformationMatrix& src)
/**
Assignment operator.

@param src: User defined transformation matrix to be copied.
@return: Self.
*/
{

	MMatrix matrix = src.asMatrix();
	MVector translation = Maxformations::matrixToPosition(matrix);
	MEulerRotation eulerRotation = Maxformations::matrixToEulerRotation(matrix, this->rotationOrder());
	MVector scale = Maxformations::matrixToScale(matrix);

	this->translateTo(translation);
	this->rotateTo(eulerRotation);
	this->scaleTo(scale);

	return *this;

};


MPxTransformationMatrix& Matrix3::operator=(const MMatrix& src)
/**
Assignment operator.
In order to convert the MMatrix to a MPxTransformationMatrix, decomposeMatrix() will be called.

@param src: The matrix to copy.
@return: Self.
*/
{

	MVector translation = Maxformations::matrixToPosition(src);
	MEulerRotation eulerRotation = Maxformations::matrixToEulerRotation(src, this->rotationOrder());
	MVector scale = Maxformations::matrixToScale(src);

	this->translateTo(translation);
	this->rotateTo(eulerRotation);
	this->scaleTo(scale);

	return *this;

};


MPxTransformationMatrix& Matrix3::operator=(const MTransformationMatrix& src)
/**
Assignment operator.

@param src: Transformation matrix to be copied.
@return: Self.
*/
{

	MVector translation = src.getTranslation(MSpace::kTransform);
	MEulerRotation eulerRotation = src.eulerRotation();
	double3 scale = { 1.0, 1.0, 1.0 };
	src.getScale(scale, MSpace::kTransform);

	this->translateTo(translation);
	this->rotateTo(eulerRotation);
	this->scaleTo(scale);

	return *this;

};


void Matrix3::copyValues(MPxTransformationMatrix* src)
/**
This method should be overridden for any transform that uses more than the default transform values.
The values from the passed class (which should be type checked and downcast to its appropriate value) should be copied to this class.
Without this method being overridden, only the default transform components will get copied.

@param src: The user defined transformation matrix that should be copied. 
@return: Void.
*/
{

	this->translateTo(src->translation());
	this->rotateTo(src->eulerRotation());
	this->scaleTo(src->scale());

};


void Matrix3::copyValues(MTransformationMatrix& src)
/**
Copies the the supplied transformation matrix.

@param src: The transformation matrix that should be copied.
@return: Void.
*/
{

	this->matrixValue = src;  // Strictly used by `Maxform::computeLocalTransformation` method!

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

	return this->matrixValue.rotationOrientation();

};


MMatrix Matrix3::asTranslateMatrix() const
/**
Returns the rotate section of the transformation matrix.

@return: The rotate matrix.
*/
{

	return Maxformations::createPositionMatrix(this->matrixValue.asMatrix());

};


MMatrix Matrix3::asRotateMatrix() const
/**
Returns the rotate section of the transformation matrix.

@return: The rotate matrix.
*/
{

	return this->matrixValue.asRotateMatrix();

};


MMatrix Matrix3::asScaleMatrix() const
/**
Returns scale matrix.
The scale matrix takes points from object space to the space immediately following scale and shear transformations.

@return: The scale matrix.
*/
{
	
	return this->matrixValue.asScaleMatrix();

};


MMatrix Matrix3::asMatrix() const
/**
Returns the four by four matrix that describes this transformation.

@return: Four by four matrix.
*/
{

	return this->matrixValue.asMatrix();

};


MTransformationMatrix Matrix3::asTransformationMatrix() const
/**
Returns the custom transformation matrix as a standard MTransformationMatrix.

@return: The transformation matrix.
*/
{

	return MTransformationMatrix(this->matrixValue);

};