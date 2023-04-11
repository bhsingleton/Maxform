#ifndef _IK_CONTROL_NODE
#define _IK_CONTROL_NODE
//
// File: IKControl.h
//
// Dependency Graph Node: ikControl
//
// Author: Benjamin H. Singleton
//

#include "Matrix3Controller.h"
#include "IKChainControl.h"
#include "SplineIKChainControl.h"
#include "PRS.h"

#include <maya/MObject.h>
#include <maya/MObjectHandle.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>
#include <math.h>


class IKChainControl;  // Forward declaration for evaluating legal connections!
class SplineIKChainControl;  // Forward declaration for evaluating legal connections!


struct IKControlSpec
{

	MEulerRotation preferredRotation = MEulerRotation::identity;  // Default rest pose
	MEulerRotation offsetRotation = MEulerRotation::identity;  // Offset rotation
	MMatrix matrix = MMatrix::identity;  // Local transform matrix
	double length = 0.0;  // Length of bone

};


class IKControl : public Matrix3Controller
{

public:

						IKControl();
	virtual				~IKControl();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	virtual	void		getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const;

	virtual	MStatus		legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal);
	virtual	MStatus		connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus		connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	virtual	bool		isAbstractClass() const;
	static  void*		creator();
	static  MStatus		initialize();

public:
	
	static	MObject		ikSubControl;
	static	MObject		fkSubControl;
	static	MObject		preferredRotation;
	static	MObject		preferredRotationX;
	static	MObject		preferredRotationY;
	static	MObject		preferredRotationZ;
	
	static	MString		inputCategory;

	static	MTypeId		id;
	
protected:

			bool		ikEnabled;
			PRS*		prs;

};

#endif