#ifndef _MAXFORMATIONS
#define _MAXFORMATIONS
//
// File: Maxformations.h
//
// Author: Benjamin H. Singleton
//

#define _USE_MATH_DEFINES

#include <maya/MObject.h>
#include <maya/MDistance.h>
#include <maya/MAngle.h>
#include <maya/MString.h>
#include <maya/MFloatArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MDataHandle.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MDGModifier.h>
#include <maya/MFileIO.h>


namespace Maxformations
{

	enum class AxisOrder
	{

		xyz = 1,
		xzy = 2,
		yzx = 3,
		yxz = 4,
		zxy = 5,
		zyx = 6,
		xyx = 7,
		yzy = 8,
		zxz = 9

	};

	template<class N> N clamp(const N& value, const N& min, const N& max)  // Template functions must go inside the header file!
	/**
	Clamps the supplied number between the specified range.

	@param value: The value to clamp.
	@param min: The minimum value.
	@param max: The maximum value.
	@return: The clamped value.
	*/
	{

		return (value < min) ? min : (value > max) ? max : value;

	};

	template<class N> N lerp(const N& start, const N& end, const double weight)  // Template functions must go inside the header file!
	/**
	Linearly interpolates the two given numbers using the supplied weight.

	@param start: The start number.
	@param end: The end number.
	@param weight: The amount to blend.
	@return: The interpolated value.
	*/
	{

		return (start * (1.0 - weight)) + (end * weight);

	};

	MMatrix			createPositionMatrix(const double x, const double y, const double z);
	MMatrix			createPositionMatrix(const MVector& position);
	MMatrix			createPositionMatrix(const MMatrix& matrix);

	MMatrix			createRotationMatrix(const double x, const double y, const double z, const AxisOrder axisOrder);
	MMatrix			createRotationMatrix(const MVector& radians, const AxisOrder axisOrder);
	MMatrix			createRotationMatrix(const MMatrix& matrix);

	MVector			getAxisVector(const int axis, const bool flip);
	MStatus			createAimMatrix(const MVector& forwardVector, const int forwardAxis, const MVector& upVector, const int upAxis, const MPoint& origin, MMatrix& matrix);
	MStatus			createAimMatrix(const int forwardAxis, const bool forwardAxisFlip, const int upAxis, const bool upAxisFlip, MMatrix& matrix);

	MMatrix			createScaleMatrix(const double x, const double y, const double z);
	MMatrix			createScaleMatrix(const MVector& scale);
	MMatrix			createScaleMatrix(const MMatrix& matrix);

	void			breakMatrix(const MMatrix& matrix, MVector& xAxis, MVector& yAxis, MVector& zAxis, MPoint& position);
	MMatrix			createMatrix(const MVector& xAxis, const MVector& yAxis, const MVector& zAxis, const MPoint& position);
	MMatrix			normalizeMatrix(const MMatrix& matrix);
	MMatrixArray	staggerMatrices(const MMatrixArray& matrices);
	MStatus			reorientMatrices(MMatrixArray& matrices, const int forwardAxis, const bool forwardAxisFlip, const int upAxis, const bool upAxisFlip);
	
	MVector			matrixToPosition(const MMatrix& matrix);

	MEulerRotation::RotationOrder	axisToRotationOrder(const AxisOrder axisOrder);
	AxisOrder						rotationToAxisOrder(const MEulerRotation::RotationOrder rotationOrder);

	MVector			matrixToEulerXYZ(const MMatrix& matrix);
	MVector			matrixToEulerXZY(const MMatrix& matrix);
	MVector			matrixToEulerYXZ(const MMatrix& matrix);
	MVector			matrixToEulerYZX(const MMatrix& matrix);
	MVector			matrixToEulerZXY(const MMatrix& matrix);
	MVector			matrixToEulerZYX(const MMatrix& matrix);
	MVector			matrixToEulerXYX(const MMatrix& matrix);
	MVector			matrixToEulerYZY(const MMatrix& matrix);
	MVector			matrixToEulerZXZ(const MMatrix& matrix);
	MVector			matrixToEulerAngles(const MMatrix& matrix, const AxisOrder axisOrder);

	MEulerRotation	matrixToEulerRotation(const MMatrix& matrix, const AxisOrder axisOrder);
	MEulerRotation	matrixToEulerRotation(const MMatrix& matrix, const MEulerRotation::RotationOrder rotationOrder);
	MEulerRotation	matrixToEulerRotation(const MMatrix& matrix, const MTransformationMatrix::RotationOrder rotationOrder);

	MQuaternion		eulerAnglesToQuaternion(const MVector& radians, const AxisOrder axisOrder);
	MQuaternion		matrixToQuaternion(const MMatrix& matrix);
	double			dot(const MQuaternion& quat, const MQuaternion& otherQuat);
	MQuaternion		slerp(const MQuaternion& startQuat, const MQuaternion& endQuat, const float weight);
	MMatrix			slerp(const MMatrix& startMatrix, const MMatrix& endMatrix, const float weight);

	MVector			matrixToScale(const MMatrix& matrix);

	MDistance		distanceBetween(const MMatrix& startMatrix, const MMatrix& endMatrix);
	MAngle			angleBetween(const MMatrix& startMatrix, const MMatrix& endMatrix);

	MObject					createMatrixData(const MMatrix& matrix);
	MObject					createMatrixData(const MTransformationMatrix& transform);
	MMatrix					getMatrixData(const MObject& matrixData);
	MTransformationMatrix	getTransformData(const MObject& matrixData);

	MStatus			resetMatrixPlug(MPlug& plug);

	bool			isPartiallyConnected(const MPlug& plug, const bool asDst, const bool asSrc, MStatus* status);
	MStatus			disconnectPlugs(const MPlug& plug, const MPlug& otherPlug);
	MStatus			breakConnections(const MPlug& plug, bool asDst, bool asSrc);
	MStatus			connectPlugs(const MPlug& plug, const MPlug& otherPlug, const bool force);

	MStatus			transferConnections(const MPlug& plug, const MPlug& otherPlug);
	MStatus			transferValues(MPlug& plug, MPlug& otherPlug);

	bool			hasTypeId(const MObject& node, const MTypeId& id, MStatus* status);
	bool			isSceneLoading();

};
#endif