#ifndef _ORIENTATION_CONSTRAINT_NODE
#define _ORIENTATION_CONSTRAINT_NODE
//
// File: PositionConstraint.h
//
// Dependency Graph Node: orientationConstraint
//
// Author: Ben Singleton
//

#include <maya/MPxConstraint.h>
#include <maya/MGlobal.h>
#include <maya/MTypeId.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDistance.h>
#include <maya/MAngle.h>
#include <maya/MVector.h>
#include <maya/MEulerRotation.h>
#include <maya/MQuaternion.h>
#include <maya/MFloatArray.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnData.h>
#include <maya/MFnMatrixData.h>

#include "Maxformations.h"


class OrientationConstraint : public MPxConstraint
{

public:

						OrientationConstraint();
	virtual				~OrientationConstraint();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	static  void*		creator();
	static  MStatus		initialize();

	const	MObject		targetAttribute() const override;
	const	MObject		weightAttribute() const override;
	const	MObject		constraintRotateOrderAttribute() const override;

public:

	static	MObject		localOrWorld;
	static	MObject		relative;
	static	MObject		offsetRotate;
	static	MObject		offsetRotateX;
	static	MObject		offsetRotateY;
	static	MObject		offsetRotateZ;
	static	MObject		restRotate;
	static	MObject		restRotateX;
	static	MObject		restRotateY;
	static	MObject		restRotateZ;

	static	MObject		target;
	static	MObject		targetWeight;
	static	MObject		targetMatrix;
	static	MObject		targetParentMatrix;
	static	MObject		targetOffsetRotate;
	static	MObject		targetOffsetRotateX;
	static	MObject		targetOffsetRotateY;
	static	MObject		targetOffsetRotateZ;

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

	static	MTypeId		id;

};
#endif