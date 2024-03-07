#ifndef _ATTACHMENT_CONSTRAINT_NOODE
#define _ATTACHMENT_CONSTRAINT_NOODE
//
// File: AttachmentConstraint.h
//
// Dependency Graph Node: attachmentConstraint
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


class AttachmentConstraint : public MPxConstraint
{

public:

						AttachmentConstraint();
	virtual				~AttachmentConstraint();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	static  void*		creator();
	static  MStatus		initialize();

	const	MObject		targetAttribute() const override;
	const	MObject		weightAttribute() const override;
	const	MObject		constraintRotateOrderAttribute() const override;

public:

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
	static  MObject		targetFace;
	static  MObject		targetCoord;
	static  MObject		targetCoordX;
	static  MObject		targetCoordY;
	static  MObject		targetMesh;

	static	MObject		constraintTranslate;
	static	MObject		constraintTranslateX;
	static	MObject		constraintTranslateY;
	static	MObject		constraintTranslateZ;
	static	MObject		constraintRotate;
	static	MObject		constraintRotateX;
	static	MObject		constraintRotateY;
	static	MObject		constraintRotateZ;
	static	MObject		constraintRotateOrder;
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