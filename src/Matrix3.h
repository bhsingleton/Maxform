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


enum class MatrixStatus
{

	Passive = 0,
	Overriden = 1,
	Exporting = 2

};


class Matrix3 : public MPxTransformationMatrix
{
	
public:
	
										Matrix3();
	virtual								~Matrix3();
	
	static	MPxTransformationMatrix*	creator();

	virtual	MQuaternion					preRotation() const override;
	virtual	MMatrix						asMatrix() const override;
	virtual	MMatrix						asRotateMatrix() const override;
	virtual	MMatrix						asScaleMatrix() const override;
	virtual	MTransformationMatrix		asTransformationMatrix() const override;

	virtual	void						setPreRotation(const MQuaternion& preRotate);
	virtual	void						setTransform(const MMatrix& matrix);
	virtual	void						pushTransform();

	virtual	bool						isEnabled();
	virtual	void						enable();
	virtual	void						disable();

	static	MTypeId						id;
	
protected:
			
			bool						enabled;
			MQuaternion					preRotationValue;
			MMatrix						transformValue;

};
#endif