#ifndef _MAXFORM_NODE
#define _MAXFORM_NODE
//
// File: Maxform.h
//
// Dependency Graph Node: maxform
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "Matrix3.h"

#include <maya/MPxTransform.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MObjectHandle.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericData.h>
#include <maya/MEvaluationNode.h>
#include <maya/MNodeCacheDisablingInfo.h>
#include <maya/MNodeCacheSetupInfo.h>
#include <maya/MSceneMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>

#include <assert.h>
#include <map>


class Maxform : public MPxTransform
{

public:

							Maxform();
	virtual					~Maxform();

	virtual MStatus			compute(const MPlug& plug, MDataBlock& data);
	virtual	MStatus			computeLocalTransformation(MPxTransformationMatrix* xform, MDataBlock& data);
	virtual bool			treatAsTransform() const;

	virtual	MStatus			validateAndSetValue(const MPlug& plug, const MDataHandle& handle);
	virtual	MStatus			connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus			connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	virtual	Matrix3*		matrix3Ptr();

	static  void*						creator();
	virtual	MPxTransformationMatrix*	createTransformationMatrix();
	static  MStatus						initialize();
	
public:
	
	static	MObject			preRotate;
	static	MObject			preRotateX;
	static	MObject			preRotateY;
	static	MObject			preRotateZ;
	static	MObject			transform;

	static	MObject			translationPart;
	static	MObject			rotationPart;
	static	MObject			scalePart;

	static	MString			preRotateCategory;
	static	MString			matrixCategory;
	static	MString			matrixPartsCategory;
	static	MString			parentMatrixCategory;
	static	MString			worldMatrixCategory;

	static	MString			classification;
	static	MTypeId			id;

private:

			bool			matrix3Enabled;

};
#endif