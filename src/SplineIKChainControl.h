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
	virtual					~SplineIKChainControl();

	virtual MStatus			compute(const MPlug& plug, MDataBlock& data);
	
	static	std::vector<IKControlSpec>	getJoints(MDataBlock& data, MStatus* status);
	static	double			getChainLength(const std::vector<IKControlSpec>& joints);
	static	MStatus			getSplineSamples(const MObject& splineShape, unsigned int numSamples, const double chainLength, MPointArray& samples);

	virtual	MStatus			solve(const MObject& splineShape, const MVector& upVector, const MAngle& startTwistAngle, const MAngle& endTwistAngle, const std::vector<IKControlSpec>& joints, MMatrixArray& matrices);
	static	MStatus			findSolution(const MPointArray& points, const std::vector<IKControlSpec>& joints, MPointArray& solution);
	static	MPointArray		lineSphereIntersection(const MPoint& startPoint, const MPoint& endPoint, const MPoint& center, const double radius);
	static	void			debugIntersection(const unsigned int index, const MPoint& startPoint, const MPoint& endPoint, const MPoint& center, const double radius, const MPointArray& hit);

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
	static	MObject			splineShape;
	static	MObject			startTwistAngle;
	static	MObject			endTwistAngle;
	static	MObject			upNode;
	static	MObject			useUpNode;

	static	MObject			goal;

	static	MString			inputCategory;
	static	MString			goalCategory;

	static	MTypeId			id;

};

#endif