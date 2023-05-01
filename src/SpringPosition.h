#ifndef _SPRING_POSITION_NODE
#define _SPRING_POSITION_NODE
//
// File: SpringPosition.h
//
// Dependency Graph Node: springPosition
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "PositionController.h"

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MDistance.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>


struct SpringConfig
{

	double mass = 300.0;
	double drag = 1.0;
	double tension = 2.0;
	double dampening = 0.5;

};


struct SpringCache
{

	MTime time;
	double velocity;
	MVector position;

};


class SpringPosition : public PositionController
{

public:

						SpringPosition();
	virtual				~SpringPosition();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	virtual	bool		isAbstractClass() const;
	static  void*		creator();
	static  MStatus		initialize();

public:

	static	MObject		absolute;
	static	MObject		mass;
	static	MObject		drag;
	static	MObject		tension;  // Also known as stiffness
	static	MObject		dampening; //  Also known as friction
	static	MObject		position;
	static	MObject		x_position;
	static	MObject		y_position;
	static	MObject		z_position;
	static	MObject		startTime;
	static	MObject		time;
	static	MObject		steps;
	static	MObject		effect;
	static	MObject		x_effect;
	static	MObject		y_effect;
	static	MObject		z_effect;
	static	MObject		parentInverseMatrix;

	static	MObject		matrix;
	static	MObject		inverseMatrix;

	static	MString		springCategory;
	static	MString		positionCategory;
	static	MString		effectCategory;

	static	MTypeId		id;

};
#endif