#ifndef _LOOK_AT_CONSTRAINT_NODE
#define _LOOK_AT_CONSTRAINT_NODE
//
// File: LookAtConstraint.h
//
// Dependency Graph Node: lookAtConstraint
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


struct UpNodeSettings
{

	MMatrix		upNodeMatrix;
	bool		upNodeWorld;
	int			upNodeControl;
	int			upNodeAxis;

};


class LookAtConstraint : public MPxConstraint
{

public:

						LookAtConstraint();
	virtual				~LookAtConstraint();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	static  void*		creator();
	static  MStatus		initialize();

	const	MObject		targetAttribute() const override;
	const	MObject		weightAttribute() const override;
	const	MObject		constraintRotateOrderAttribute() const override;

	static	MStatus		getUpVector(const UpNodeSettings& settings, const MVector& origin, MVector& upVector);

public:

	static	MObject		targetAxis;
	static	MObject		targetAxisFlip;
	static	MObject		roll;
	static  MObject		sourceUpAxis;
	static  MObject		sourceUpAxisFlip;
	static	MObject		upNode;
	static	MObject		upNodeWorld;
	static	MObject		upNodeControl;
	static  MObject		upNodeAxis;
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