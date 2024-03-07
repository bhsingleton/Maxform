#ifndef _POSITION_CONSTRAINT_NODE
#define _POSITION_CONSTRAINT_NODE
//
// File: PositionConstraint.h
//
// Dependency Graph Node: positionConstraint
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


class PositionConstraint : public MPxConstraint
{

public:

						PositionConstraint();
	virtual				~PositionConstraint();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	static  void*		creator();
	static  MStatus		initialize();

	const	MObject		targetAttribute() const override;
	const	MObject		weightAttribute() const override;

public:

	static	MObject		localOrWorld;
	static	MObject		relative;
	static	MObject		offsetTranslate;
	static	MObject		offsetTranslateX;
	static	MObject		offsetTranslateY;
	static	MObject		offsetTranslateZ;
	static	MObject		restTranslate;
	static	MObject		restTranslateX;
	static	MObject		restTranslateY;
	static	MObject		restTranslateZ;

	static	MObject		target;
	static	MObject		targetWeight;
	static	MObject		targetMatrix;
	static	MObject		targetParentMatrix;
	static	MObject		targetOffsetTranslate;
	static	MObject		targetOffsetTranslateX;
	static	MObject		targetOffsetTranslateY;
	static	MObject		targetOffsetTranslateZ;

	static	MObject		constraintTranslate;
	static	MObject		constraintTranslateX;
	static	MObject		constraintTranslateY;
	static	MObject		constraintTranslateZ;
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