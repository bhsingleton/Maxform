#ifndef _IK_CHAIN_CONTROL_NODE
#define _IK_CHAIN_CONTROL_NODE
//
// File: IKChainControl.h
//
// Dependency Graph Node: ikChainControl
//
// Author: Benjamin H. Singleton
//

#include "Matrix3Controller.h"
#include "PRS.h"

#include <maya/MObject.h>
#include <maya/MObjectHandle.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
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

#include <map>
#include <vector>


struct JointItem
{

	MEulerRotation preferredRotation = MEulerRotation::identity;
	MMatrix matrix = MMatrix::identity;

};


class IKChainControl : public Matrix3Controller
{

public:

									IKChainControl();
	virtual							~IKChainControl();

	virtual MStatus					compute(const MPlug& plug, MDataBlock& data);
	
	static	std::vector<JointItem>	getJoints(MArrayDataHandle& arrayHandle);
	static	MMatrix					guessVHTarget(const std::vector<JointItem>& joints, const int upAxis, const bool upAxisFlip);
	static	MMatrixArray			solve(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const std::vector<JointItem>& joints);
	static	MMatrixArray			solve1Bone(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const JointItem& startJoint, const JointItem& endJoint);
	static	MMatrixArray			solve2Bone(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const JointItem& startJoint, const JointItem& midJoint, const JointItem& endJoint);
	static	MMatrixArray			solveNBone(const MMatrix& ikGoal, const MMatrix& vhTarget, const MAngle& swivelAngle, const std::vector<JointItem>& joints);

	virtual	MStatus					legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal);
	virtual	MStatus					connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus					connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	virtual	bool					isAbstractClass() const;
	static  void*					creator();
	static  MStatus					initialize();

public:


	static	MObject		enabled;
	static	MObject		ikGoal;
	static	MObject		forwardAxis;
	static	MObject		forwardAxisFlip;
	static	MObject		upAxis;
	static	MObject		upAxisFlip;
	static	MObject		joint;
	static	MObject		jointPreferredRotation;
	static	MObject		jointPreferredRotationX;
	static	MObject		jointPreferredRotationY;
	static	MObject		jointPreferredRotationZ;
	static	MObject		jointMatrix;
	static	MObject		swivelAngle;
	static	MObject		useVHTarget;
	static	MObject		vhTarget;

	static	MObject		goal;

	static	MString		inputCategory;
	static	MString		goalCategory;

	static	MTypeId		id;

protected:

			PRS*		prs;

};

#endif