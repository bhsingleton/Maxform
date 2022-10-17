#ifndef _PRSNode
#define _PRSNode
//
// File: PRSNode.h
//
// Dependency Graph Node: prs
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "Maxform.h"

#include <utility>
#include <map>
#include <vector>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericData.h>
#include <maya/MNodeMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>


class PRS : public MPxNode
{

public:

						PRS();
	virtual				~PRS();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);
	
	virtual MStatus		connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual MStatus		connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	virtual	Maxform*	maxformPtr();
	virtual	void		updateMaxformPtr(Maxform* maxform);
	
	static  void*		creator();
	static  MStatus		initialize();

public:

	static	MObject		position;
	static	MObject		x_position;
	static	MObject		y_position;
	static	MObject		z_position;
	static	MObject		rotation;
	static	MObject		x_rotation;
	static	MObject		y_rotation;
	static	MObject		z_rotation;
	static	MObject		w_rotation;
	static	MObject		axisOrder;
	static	MObject		euler_rotation;
	static	MObject		euler_x_rotation;
	static	MObject		euler_y_rotation;
	static	MObject		euler_z_rotation;
	static	MObject		scale;
	static	MObject		x_scale;
	static	MObject		y_scale;
	static	MObject		z_scale;
	
	static	MObject		matrix;
	static	MObject		inverseMatrix;

public:

	static	MTypeId		id;

	static	MString		inputCategory;
	static	MString		outputCategory;
	static	MString		positionCategory;
	static	MString		rotationCategory;
	static	MString		eulerRotationCategory;
	static	MString		scaleCategory;

protected:
			
			Maxform*		maxform;

};
#endif