#ifndef _PositionListNode
#define _PositionListNode
//
// File: PositionListNode.h
//
// Dependency Graph Node: positionList
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
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericData.h>
#include <maya/MTypeId.h> 
#include <maya/MFileIO.h>
#include <maya/MGlobal.h>

 
struct PositionListItem
{

	MString	name = "";
	float weight = 1.0;
	bool absolute = false;
	MVector translate = MVector::zero;

};


class PositionList : public MPxNode
{

public:

						PositionList();
	virtual				~PositionList();

	virtual MStatus		compute(const MPlug& plug, MDataBlock& data);

	virtual	bool		setInternalValue(const MPlug& plug, const MDataHandle& handle);
	virtual	MStatus		updateActiveController(MPlug& translatePlug);

	static  void*		creator();
	static  MStatus		initialize();

	static	MVector		sum(MArrayDataHandle& handle, const bool normalizeWeights, MStatus* status);
	static	MVector		sum(MArrayDataHandle& handle, const unsigned int active, const bool normalizeWeights, MStatus* status);
	static	MVector		sum(std::vector<PositionListItem>& items, const bool normalizeWeights);
	static	void		normalize(std::vector<PositionListItem>& items);
	
public:

	static	MObject		active;
	static	MObject		average;
	static	MObject		list;
	static	MObject		name;
	static	MObject		weight;
	static	MObject		absolute;
	static	MObject		position;
	static	MObject		x_position;
	static	MObject		y_position;
	static	MObject		z_position;
	
	static	MObject		value;
	static	MObject		valueX;
	static	MObject		valueY;
	static	MObject		valueZ;
	static	MObject		preValue;
	static	MObject		preValueX;
	static	MObject		preValueY;
	static	MObject		preValueZ;
	static	MObject		matrix;
	static	MObject		inverseMatrix;

	static	MString		inputCategory;
	static	MString		outputCategory;
	static	MString		listCategory;
	static	MString		positionCategory;
	static	MString		prePositionCategory;

	static	MTypeId		id;

protected:

			unsigned int	previousIndex;
			unsigned int	activeIndex;

};
#endif