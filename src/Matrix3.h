#ifndef _MATRIX3
#define _MATRIX3
//
// File: Matrix3.h
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"

#include <maya/MPxTransformationMatrix.h>
#include <maya/MVector.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>
#include <math.h>


class Matrix3 : public MPxTransformationMatrix
{
	
public:
	
										Matrix3();
	virtual								~Matrix3();
	
	static	MPxTransformationMatrix*	creator();

	virtual	MPxTransformationMatrix&	operator=(const MPxTransformationMatrix& src);
	virtual	MPxTransformationMatrix&	operator=(const MMatrix& src);
	virtual	MPxTransformationMatrix&	operator=(const MTransformationMatrix& src);
	virtual	void						copyValues(MPxTransformationMatrix* src);
	virtual	void						copyValues(MTransformationMatrix& src);

	virtual	MQuaternion					preRotation() const override;
	virtual	MMatrix						asMatrix() const override;
	virtual	MMatrix						asTranslateMatrix() const;
	virtual	MMatrix						asRotateMatrix() const override;
	virtual	MMatrix						asScaleMatrix() const override;
	virtual	MTransformationMatrix		asTransformationMatrix() const override;

	static	MTypeId						id;
	
protected:
			
			MTransformationMatrix		matrixValue;

};
#endif