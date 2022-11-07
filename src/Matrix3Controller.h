#ifndef _MATRIX3_CONTROLLER
#define _MATRIX3_CONTROLLER
//
// File: Matrix3Controller.h
//
// Author: Benjamin H. Singleton
//

#include "Maxform.h"

#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>


class Matrix3Controller : public MPxNode
{

public:

						Matrix3Controller();
	virtual				~Matrix3Controller();

	virtual	MStatus		compute(const MPlug& plug, MDataBlock& data);

	virtual	MStatus		connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus		connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	virtual	bool		isAbstractClass() const;
	static  void*		creator();
	static  MStatus		initialize();

	virtual	Maxform*	maxformPtr();

public:

	static	MObject		IDENTITY_MATRIX_DATA;

public:

	static	MObject		value;
	static	MString		valueCategory;
	
	static	MTypeId		id;

protected:
			
			Maxform*	maxform;
};

#endif