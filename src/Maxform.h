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

#include <assert.h>
#include <map>

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
#include <maya/MDGContext.h>
#include <maya/MDGContextGuard.h>
#include <maya/MEvaluationNode.h>
#include <maya/MNodeCacheDisablingInfo.h>
#include <maya/MNodeCacheSetupInfo.h>
#include <maya/MSceneMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>


class Maxform : public MPxTransform
{

public:

							Maxform();
	virtual					~Maxform();

	virtual MStatus			compute(const MPlug& plug, MDataBlock& data);
	virtual	MStatus			computeLocalTransformation(MPxTransformationMatrix* xform, MDataBlock& data);
	virtual bool			treatAsTransform() const;

	virtual	void			getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const;
	virtual	MStatus			validateAndSetValue(const MPlug& plug, const MDataHandle& handle);

	static  void*						creator();
	virtual	MPxTransformationMatrix*	createTransformationMatrix();
	
	static  MStatus			initialize();
	
public:
	
	static	MObject			transform;

	static	MObject			translationPart;
	static	MObject			rotationPart;
	static	MObject			scalePart;

	static	MString			matrixCategory;
	static	MString			matrixPartsCategory;
	static	MString			parentMatrixCategory;
	static	MString			worldMatrixCategory;

	static	MString			classification;
	static	MTypeId			id;

};
#endif