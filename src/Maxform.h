#ifndef _Maxform
#define _Maxform
//
// File: Maxform.h
//
// Dependency Graph Node: maxform
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "Matrix3.h"

#include <maya/MPxTransform.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericData.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>
#include <math.h>


class Maxform : public MPxTransform
{

public:

										Maxform();
	virtual								~Maxform();

	virtual	MPxTransformationMatrix*	createTransformationMatrix();
	virtual	Matrix3*					matrix3Ptr();
	
	virtual MStatus						compute(const MPlug& plug, MDataBlock& data);
	virtual	MStatus						validateAndSetValue(const MPlug& plug, const MDataHandle& handle) override;

	static  void*						creator();
	static  MStatus						initialize();
	
public:
	
	static	MObject		preTranslate;
	static	MObject		preTranslateX;
	static	MObject		preTranslateY;
	static	MObject		preTranslateZ;
	static	MObject		preRotate;
	static	MObject		preRotateX;
	static	MObject		preRotateY;
	static	MObject		preRotateZ;
	static	MObject		preRotateW;
	static	MObject		preScale;
	static	MObject		preScaleX;
	static	MObject		preScaleY;
	static	MObject		preScaleZ;
	static	MObject		transform;

	static	MObject		translationPart;
	static	MObject		rotationPart;
	static	MObject		scalePart;

	static	MString		inputCategory;
	static	MString		outputCategory;
	static	MString		preTranslateCategory;
	static	MString		preRotateCategory;
	static	MString		preScaleCategory;

	static	MTypeId		id;
	static	MString		classification;

};
#endif