#ifndef _ROTATION_LIST_NODE
#define _ROTATION_LIST_NODE
//
// File: RotationList.h
//
// Dependency Graph Node: rotationList
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"
#include "PRS.h"

#include <utility>
#include <map>
#include <vector>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericData.h>
#include <maya/MTypeId.h> 
#include <maya/MGlobal.h>
#include <math.h>


struct RotationListItem
{

	MString	name = "";
	float weight = 1.0;
	bool absolute = false;
	MEulerRotation eulerRotation = MEulerRotation::identity;

};


class RotationList : public MPxNode
{

public:

							RotationList();
	virtual					~RotationList();

	virtual MStatus			compute(const MPlug& plug, MDataBlock& data);
	
	virtual	bool			setInternalValue(const MPlug& plug, const MDataHandle& handle);
	virtual MStatus			connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc);
	virtual MStatus			connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc);

	static	MQuaternion		sum(MArrayDataHandle& handle, const bool normalizeWeights, MStatus* status);
	static	MQuaternion		sum(MArrayDataHandle& handle, const unsigned int active, const bool normalizeWeights, MStatus* status);
	static	MQuaternion		sum(std::vector<RotationListItem>& items, const bool normalizeWeights);
	static	void			normalize(std::vector<RotationListItem>& items);

	virtual	MStatus			updateActiveController();
	virtual	MStatus			pullController(unsigned int index);
	virtual	MStatus			pushController(unsigned int index);

	virtual	Maxform*		maxformPtr();

	static  void*			creator();
	static  MStatus			initialize();

public:

	static	MObject			active;
	static	MObject			average;
	static	MObject			list;
	static	MObject			name;
	static	MObject			weight;
	static	MObject			absolute;
	static	MObject			axisOrder;
	static	MObject			rotation;
	static	MObject			x_rotation;
	static	MObject			y_rotation;
	static	MObject			z_rotation;
	
	static	MObject			value;
	static	MObject			valueX;
	static	MObject			valueY;
	static	MObject			valueZ;
	static	MObject			preValue;
	static	MObject			preValueX;
	static	MObject			preValueY;
	static	MObject			preValueZ;
	static	MObject			matrix;
	static	MObject			inverseMatrix;

	static	MString			inputCategory;
	static	MString			outputCategory;
	static	MString			listCategory;
	static	MString			rotationCategory;
	static	MString			preRotationCategory;

	static	MTypeId			id;
	
public:



protected:
			
			PRS*			prs;
			unsigned int	previousIndex;;
			unsigned int	activeIndex;

};
#endif