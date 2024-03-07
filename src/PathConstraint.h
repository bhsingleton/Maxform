#ifndef _PATH_CONSTRAINT_NODE
#define _PATH_CONSTRAINT_NODE
//
// File: PathConstraint.h
//
// Dependency Graph Node: pathConstraint
//
// Author: Benjamin H. Singleton
//

#include <maya/MPxConstraint.h>
#include <maya/MObject.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MPlug.h>
#include <maya/MDistance.h>
#include <maya/MAngle.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MFloatArray.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MString.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MTypeId.h>
#include <maya/MGlobal.h>

#include "Maxformations.h"


enum class WorldUpType
{

	SceneUp = 0,
	ObjectUp = 1,
	ObjectRotationUp = 2,
	Vector = 3,
	CurveNormal = 4

};


struct WorldUpSettings
{

	WorldUpType		worldUpType;
	MVector			worldUpVector;
	MMatrix			worldUpMatrix;

};


struct AxisSettings
{

	int				forwardAxis;
	bool			forwardAxisFlip;
	int				upAxis;
	bool			upAxisFlip;
	MAngle			twistAngle;
	WorldUpSettings	worldUpSettings;

};


class PathConstraint : public MPxConstraint
{

public:

						PathConstraint();
	virtual				~PathConstraint();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	static  void*		creator();
	static  MStatus		initialize();

	const	MObject		targetAttribute() const override;
	const	MObject		weightAttribute() const override;
	const	MObject		constraintRotateOrderAttribute() const override;

	static	MStatus		createMatrixFromCurve(const MObject& curve, const double parameter, const AxisSettings& settings, MMatrix& matrix);
	static	double		clampCurveParameter(const MObject& curve, const double parameter, MStatus* status);
	static	MStatus		getForwardVector(const MObject& curve, const double parameter, MVector& forwardVector);
	static	MStatus		getUpVector(const MObject& curve, const double parameter, const WorldUpSettings& settings, const MVector& origin, MVector& upVector);
	static	MVector		getObjectRotationUpVector(const MVector& worldUpVector, const MMatrix& worldUpMatrix);
	static	MStatus		getCurvePoint(const MObject& curve, const double parameter, MPoint& point);
	static	MStatus		getCurveNormal(const MObject& curve, const double parameter, MVector& upVector);

public:

	static	MObject		percent;
	static	MObject		loop;
	static	MObject		forwardAxis;
	static	MObject		forwardAxisFlip;
	static	MObject		twist;
	static  MObject		upAxis;
	static  MObject		upAxisFlip;
	static  MObject		worldUpType;
	static  MObject		worldUpVector;
	static  MObject		worldUpVectorX;
	static  MObject		worldUpVectorY;
	static  MObject		worldUpVectorZ;
	static  MObject		worldUpMatrix;
	static	MObject		relative;
	static	MObject		offsetTranslate;
	static	MObject		offsetTranslateX;
	static	MObject		offsetTranslateY;
	static	MObject		offsetTranslateZ;
	static	MObject		offsetRotate;
	static	MObject		offsetRotateX;
	static	MObject		offsetRotateY;
	static	MObject		offsetRotateZ;
	static	MObject		restTranslate;
	static	MObject		restTranslateX;
	static	MObject		restTranslateY;
	static	MObject		restTranslateZ;
	static	MObject		restRotate;
	static	MObject		restRotateX;
	static	MObject		restRotateY;
	static	MObject		restRotateZ;

	static  MObject		target;
	static  MObject		targetWeight;
	static  MObject		targetCurve;

	static	MObject		constraintTranslate;
	static	MObject		constraintTranslateX;
	static	MObject		constraintTranslateY;
	static	MObject		constraintTranslateZ;
	static	MObject		constraintRotateOrder;
	static	MObject		constraintRotate;
	static	MObject		constraintRotateX;
	static	MObject		constraintRotateY;
	static	MObject		constraintRotateZ;
	static	MObject		constraintMatrix;
	static	MObject		constraintInverseMatrix;
	static	MObject		constraintWorldMatrix;
	static	MObject		constraintWorldInverseMatrix;
	static	MObject		constraintParentInverseMatrix;

public:

	static	MString		inputCategory;
	static	MString		offsetCategory;
	static	MString		restCategory;
	static	MString		targetCategory;
	static	MString		outputCategory;

	static	MString		classification;

	static	MTypeId		id;

};

#endif