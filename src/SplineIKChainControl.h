#ifndef _SPLINE_IK_CHAIN_CONTROL_NODE
#define _SPLINE_IK_CHAIN_CONTROL_NODE
//
// File: SplineIKChainControl.h
//
// Dependency Graph Node: splineIKChainControl
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
#include <maya/MFnNurbsCurve.h>
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
struct IKControlSpec;  // Forward declaration for storing joint data!


class SplineIKChainControl : public Matrix3Controller
{

public:

									SplineIKChainControl();
	virtual							~SplineIKChainControl();

	virtual MStatus					compute(const MPlug& plug, MDataBlock& data);
	
	static	std::vector<IKControlSpec>	getJoints(MArrayDataHandle& arrayHandle);
	static	MVector						getUpVector(const MObject& splineShape, const MMatrix& upNode, MStatus* status);

	virtual	MStatus					solve(const MObject& splineShape, const MVector& upVector, const MAngle& startTwistAngle, const MAngle& endTwistAngle, const std::vector<IKControlSpec>& joints, MMatrixArray& matrices);
	virtual	MStatus					refineSolution(const MObject& splineShape, const MPoint& origin, const double boneLength, MPoint& point, double& curveLength);
	virtual	MDoubleArray			getBoneLengths(const std::vector<IKControlSpec>& joints);
	virtual	MStatus					adjustPointsByLength(const MPointArray& points, const MDoubleArray& lengths, MPointArray& adjustedPoints);

	virtual	bool					setInternalValue(const MPlug& plug, const MDataHandle& handle);

	virtual	MStatus					legalConnection(const MPlug& plug, const MPlug& otherPlug, bool asSrc, bool& isLegal);
	virtual	MStatus					connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus					connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	virtual	bool					isAbstractClass() const;
	static  void*					creator();
	static  MStatus					initialize();

public:

	static	MObject					enabled;
	static	MObject					ikGoal;
	static	MObject					ikParentMatrix;
	static	MObject					forwardAxis;
	static	MObject					forwardAxisFlip;
	static	MObject					upAxis;
	static	MObject					upAxisFlip;
	static	MObject					joint;
	static	MObject					jointPreferredRotation;
	static	MObject					jointPreferredRotationX;
	static	MObject					jointPreferredRotationY;
	static	MObject					jointPreferredRotationZ;
	static	MObject					jointOffsetRotation;
	static	MObject					jointOffsetRotationX;
	static	MObject					jointOffsetRotationY;
	static	MObject					jointOffsetRotationZ;
	static	MObject					jointMatrix;
	static	MObject					jointParentMatrix;
	static	MObject					splineShape;
	static	MObject					startTwistAngle;
	static	MObject					endTwistAngle;
	static	MObject					upNode;
	static	MObject					useUpNode;
	static	MObject					iterations;  // Controls the maximum number of iterations for each solution
	static	MObject					tolerance;  // Controls the margin of error allowed for each solution

	static	MObject					goal;

	static	MString					inputCategory;
	static	MString					goalCategory;

	static	MTypeId					id;

protected:
			
			PRS*					prs;
			float					toleranceValue;
			int						iterationLimit;

};

#endif