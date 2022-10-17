#ifndef _ExposeTransform
#define _ExposeTransform
//
// File: ExposeTm.h
//
// Dependency Graph Node: exposeTm
//
// Author: Benjamin H. Singleton
//

#include "Maxform.h"

#include <maya/MObject.h>
#include <maya/MObjectHandle.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MDistance.h>
#include <maya/MAngle.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MDGModifier.h>
#include <maya/MDGContext.h>
#include <maya/MAnimControl.h>
#include <maya/MFileIO.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>
#include <math.h>


class ExposeTransform : public Maxform
{

public:

						ExposeTransform();
	virtual				~ExposeTransform();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data)
		;
	virtual	bool		setInternalValue(const MPlug& plug, const MDataHandle& dataHandle);

	virtual	MStatus		legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal);
	virtual	MStatus		connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus		connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	static  void*		creator();
	static  MStatus		initialize();

public:
	
	static	MObject		exposeNode;
	static	MObject		exposeMatrix;
	static	MObject		localReferenceNode;
	static	MObject		localReferenceMatrix;
	static	MObject		useParent;
	static	MObject		useTimeOffset;
	static	MObject		timeOffset;
	
	static	MObject		localPosition;
	static	MObject		localPositionX;
	static	MObject		localPositionY;
	static	MObject		localPositionZ;
	static	MObject		worldPosition;
	static	MObject		worldPositionX;
	static	MObject		worldPositionY;
	static	MObject		worldPositionZ;
	static	MObject		eulerXOrder;
	static	MObject		eulerYOrder;
	static	MObject		eulerZOrder;
	static	MObject		localEuler;
	static	MObject		localEulerX;
	static	MObject		localEulerY;
	static	MObject		localEulerZ;
	static	MObject		worldEuler;
	static	MObject		worldEulerX;
	static	MObject		worldEulerY;
	static	MObject		worldEulerZ;
	static	MObject		stripNUScale;
	static	MObject		distance;
	static	MObject		angle;
	
	static	MString		exposeCategory;

	static	MTypeId		id;
	static	MString		classification;
	
	
private:

			MObjectHandle	exposeHandle;
			MObjectHandle	localReferenceHandle;
			bool			parentEnabled;

	virtual	MStatus			updateExposeMatrix();
	virtual MStatus			updateLocalReferenceMatrix();

};

#endif