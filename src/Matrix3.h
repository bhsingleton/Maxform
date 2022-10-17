#ifndef _Matrix3
#define _Matrix3
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
	
	virtual	MQuaternion					preRotation() const override;
	virtual	MQuaternion					rotateOrientation(MSpace::Space space, MStatus* status) const override;

	virtual	MMatrix						asMatrix() const override;
	virtual	MMatrix						asRotateMatrix() const override;
	virtual	MMatrix						asScaleMatrix() const override;

	virtual	void						setPreTranslation(const MVector& preTranslate);
	virtual	void						setPreRotation(const MQuaternion& preRotate);
	virtual	void						setPreScale(const MVector& preScale);
	virtual	void						setTransform(const MMatrix& matrix);

	virtual	void						enableOverride();
	virtual	void						disableOverride();

public:

	static	MTypeId						id;
	
protected:
			
			MVector			preTranslateValue;
			MQuaternion		preRotateValue;
			MVector			preScaleValue;
			MMatrix			transformValue;
			bool			overrideEnabled;
	
};
#endif