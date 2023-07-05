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
#include "IKControl.h"
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


class IKControl;  // Forward declaration for evaluating legal connections!
class IKControlSpec;  // Forward declaration for storing joint data!


class IKChainControl : public Matrix3Controller
{

public:

							IKChainControl();
	virtual					~IKChainControl();

	virtual MStatus			compute(const MPlug& plug, MDataBlock& data);
	
	static	std::vector<IKControlSpec>	getJoints(MDataBlock& data, MStatus* status);
	static	MVector			getUpVector(const MMatrix& startJoint, const MMatrix& vhTarget);
	static	MVector			guessUpVector(const std::vector<IKControlSpec>& joints, const int upAxis, const bool upAxisFlip);
	static	MVector			guessUpVector(const std::vector<IKControlSpec>& joints);

	static	double			lagrange2d(const double x, const MVector& p1, const MVector& p2, const MVector& p3);
	static	MPointArray		compressPoints(const MPointArray& points, const MVector& effector);

	static	MMatrixArray	solve(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const std::vector<IKControlSpec>& joints);
	static	MMatrixArray	solve1Bone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const IKControlSpec& startJoint, const IKControlSpec& endJoint);
	static	MMatrixArray	solve2Bone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const IKControlSpec& startJoint, const IKControlSpec& midJoint, const IKControlSpec& endJoint);
	static	MMatrixArray	solveNBone(const MMatrix& ikGoal, const MVector& upVector, const MAngle& swivelAngle, const std::vector<IKControlSpec>& joints, const unsigned int iterations);

	virtual	MStatus			legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal);

	virtual	bool			isAbstractClass() const;
	static  void*			creator();
	static  MStatus			initialize();

public:

	static	MObject			enabled;
	static	MObject			ikGoal;
	static	MObject			ikParentMatrix;
	static	MObject			forwardAxis;
	static	MObject			forwardAxisFlip;
	static	MObject			upAxis;
	static	MObject			upAxisFlip;
	static	MObject			joint;
	static	MObject			jointPreferredRotation;
	static	MObject			jointPreferredRotationX;
	static	MObject			jointPreferredRotationY;
	static	MObject			jointPreferredRotationZ;
	static	MObject			jointOffsetRotation;
	static	MObject			jointOffsetRotationX;
	static	MObject			jointOffsetRotationY;
	static	MObject			jointOffsetRotationZ;
	static	MObject			jointMatrix;
	static	MObject			jointParentMatrix;
	static	MObject			swivelAngle;
	static	MObject			useVHTarget;
	static	MObject			vhTarget;  // Stands for (v)ector (h)andle target!

	static	MObject			goal;

	static	MString			inputCategory;
	static	MString			goalCategory;
	static	MTypeId			id;

};
#endif