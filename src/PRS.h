#ifndef _PRS_NODE
#define _PRS_NODE
//
// File: PRS.h
//
// Dependency Graph Node: prs
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "Matrix3Controller.h"

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


class PRS : public Matrix3Controller
{

public:

						PRS();
	virtual				~PRS();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);
	
	virtual	bool		isAbstractClass() const;
	static  void*		creator();
	static  MStatus		initialize();

	virtual	void		registerMasterController(Matrix3Controller* controller);
	virtual	void		deregisterMasterController();

	virtual	Maxform*	maxformPtr() override;

public:

	static	MObject		position;
	static	MObject		x_position;
	static	MObject		y_position;
	static	MObject		z_position;
	static	MObject		axisOrder;
	static	MObject		rotation;
	static	MObject		x_rotation;
	static	MObject		y_rotation;
	static	MObject		z_rotation;
	static	MObject		scale;
	static	MObject		x_scale;
	static	MObject		y_scale;
	static	MObject		z_scale;
	
public:

	static	MString		inputCategory;
	static	MString		positionCategory;
	static	MString		rotationCategory;
	static	MString		scaleCategory;

	static	MTypeId		id;

protected:

			bool				isSlaveController;
			Matrix3Controller*	masterController;

};
#endif