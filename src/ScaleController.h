#ifndef _SCALE_CONTROLLER
#define _SCALE_CONTROLLER
//
// File: ScaleController.h
//
// Author: Benjamin H. Singleton
//

#include "Maxform.h"
#include "Matrix3Controller.h"

#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>


class ScaleController : public MPxNode
{

public:

							ScaleController();
	virtual					~ScaleController();

	virtual	MStatus			compute(const MPlug& plug, MDataBlock& data);

	virtual	MStatus			connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual	MStatus			connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	virtual	bool			isAbstractClass() const;
	static  void*			creator();
	static  MStatus			initialize();

	virtual	void			dependentChanged(const MObject& otherNode);
	virtual	Maxform*		getAssociatedTransform(MStatus* status);

public:

	static	MObject			value;
	static	MObject			valueX;
	static	MObject			valueY;
	static	MObject			valueZ;

	static	MString			valueCategory;

	static	MString			classification;
	static	MTypeId			id;

protected:

			MObjectHandle	scaleController;
			MObjectHandle	matrix3Controller;

};
#endif