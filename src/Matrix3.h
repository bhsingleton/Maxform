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
	virtual	MTransformationMatrix		asTransformationMatrix() const override;

	virtual	void						updatePreTranslation(const MVector& preTranslate);
	virtual	void						updatePreRotation(const MQuaternion& preRotate);
	virtual	void						updatePreScale(const MVector& preScale);
	virtual	void						updateTransform(const MMatrix& matrix);

public:

	static	MTypeId						id;
	
protected:
			
			MVector			preTranslate;
			MQuaternion		preRotate;
			MVector			preScale;
			MMatrix			transform;
			bool			overrideEnabled;
	
};
#endif