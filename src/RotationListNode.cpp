//
// File: RotationListNode.cpp
//
// Dependency Graph Node: rotationList
//
// Author: Benjamin H. Singleton
//

#include "RotationListNode.h"

MObject		RotationList::active;
MObject		RotationList::average;
MObject		RotationList::list;
MObject		RotationList::name;
MObject		RotationList::weight;
MObject		RotationList::absolute;
MObject		RotationList::axisOrder;
MObject		RotationList::rotation;
MObject		RotationList::x_rotation;
MObject		RotationList::y_rotation;
MObject		RotationList::z_rotation;

MObject		RotationList::value;
MObject		RotationList::valueX;
MObject		RotationList::valueY;
MObject		RotationList::valueZ;
MObject		RotationList::valueW;
MObject		RotationList::preValue;
MObject		RotationList::preValueX;
MObject		RotationList::preValueY;
MObject		RotationList::preValueZ;
MObject		RotationList::preValueW;
MObject		RotationList::matrix;
MObject		RotationList::inverseMatrix;

MString		RotationList::inputCategory("Input");
MString		RotationList::outputCategory("Output");
MString		RotationList::listCategory("List");
MString		RotationList::rotationCategory("Rotation");
MString		RotationList::preRotationCategory("PreRotation");

MTypeId		RotationList::id(0x0013b1c6);


RotationList::RotationList() { this->previousIndex = 0; this->activeIndex = 0; };
RotationList::~RotationList() {};


MStatus RotationList::compute(const MPlug& plug, MDataBlock& data) 
/**
This method should be overridden in user defined nodes.
Recompute the given output based on the nodes inputs.
The plug represents the data value that needs to be recomputed, and the data block holds the storage for all of the node's attributes.
The MDataBlock will provide smart handles for reading and writing this node's attribute values.
Only these values should be used when performing computations!

@param plug: Plug representing the attribute that needs to be recomputed.
@param data: Data block containing storage for the node's attributes.
@return: Return status.
*/
{
	
	MStatus status;

	// Check requested attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (fnAttribute.hasCategory(RotationList::outputCategory))
	{
		
		// Get input data handles
		//
		MDataHandle averageHandle = data.inputValue(RotationList::average, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle listHandle = data.inputArrayValue(RotationList::list, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get input values
		//
		bool normalizeWeights = averageHandle.asBool();

		// Calculate weighted average
		//
		MQuaternion quat = RotationList::sum(listHandle, normalizeWeights, &status);
		MMatrix matrix = quat.asMatrix();
		
		// Get output data handles
		//
		MDataHandle valueXHandle = data.outputValue(RotationList::valueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueYHandle = data.outputValue(RotationList::valueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueZHandle = data.outputValue(RotationList::valueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueWHandle = data.outputValue(RotationList::valueW, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle matrixHandle = data.outputValue(RotationList::matrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle inverseMatrixHandle = data.outputValue(RotationList::inverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Set output handle values
		//
		valueXHandle.setDouble(quat.x);
		valueXHandle.setClean();
		
		valueYHandle.setDouble(quat.y);
		valueYHandle.setClean();
		
		valueZHandle.setDouble(quat.z);
		valueZHandle.setClean();

		valueWHandle.setDouble(quat.w);
		valueWHandle.setClean();

		matrixHandle.setMMatrix(matrix);
		matrixHandle.setClean();

		inverseMatrixHandle.setMMatrix(matrix.inverse());
		inverseMatrixHandle.setClean();

		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else if (fnAttribute.hasCategory(RotationList::preRotationCategory))
	{
		
		// Get input data handles
		//
		MDataHandle activeHandle = data.inputValue(RotationList::active, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle averageHandle = data.inputValue(RotationList::average, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle listHandle = data.inputArrayValue(RotationList::list, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get input values
		//
		unsigned int active = activeHandle.asShort();
		bool normalizeWeights = averageHandle.asBool();

		// Calculate weighted average
		//
		MQuaternion quat = RotationList::sum(listHandle, active - 1, normalizeWeights, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get output data handles
		//
		MDataHandle preValueXHandle = data.outputValue(RotationList::preValueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preValueYHandle = data.outputValue(RotationList::preValueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preValueZHandle = data.outputValue(RotationList::preValueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preValueWHandle = data.outputValue(RotationList::preValueW, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Set output handle preValues
		//
		preValueXHandle.setDouble(quat.x);
		preValueXHandle.setClean();

		preValueYHandle.setDouble(quat.y);
		preValueYHandle.setClean();

		preValueZHandle.setDouble(quat.z);
		preValueZHandle.setClean();

		preValueWHandle.setDouble(quat.w);
		preValueWHandle.setClean();

		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else
	{
		
		return MS::kUnknownParameter;

	}

};


bool RotationList::setInternalValue(const MPlug& plug, const MDataHandle& handle)
/**
This method is overridden by nodes that store attribute data in some internal format.
The internal state of attributes can be set or queried using the setInternal and internal methods of MFnAttribute.
When internal attribute values are set via setAttr or MPlug::setValue this method is called.
Another use for this method is to impose attribute limits.

@param plug: The attribute that is being set.
@param handle: The dataHandle containing the value to set.
@return: Success.
*/
{
	
	MStatus status;

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN(status, false);

	if (attribute == RotationList::active)
	{

		this->activeIndex = handle.asShort();

	}

	return MPxNode::setInternalValue(plug, handle);;

};


MStatus RotationList::updateActiveController(MPlug& rotatePlug)
/**
Updates the active controller.

@param activeIndex: The new active index.
@return: Status code.
*/
{

	MStatus status;

	// Redundancy check
	//
	if (this->previousIndex == this->activeIndex)
	{

		return MS::kSuccess;  // Nothing to do here~!

	}

	// Get list plug elements
	//
	MPlug listPlug = MPlug(this->thisMObject(), RotationList::list);

	MPlug previousElement = listPlug.elementByLogicalIndex(this->previousIndex, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlug newElement = listPlug.elementByLogicalIndex(this->activeIndex, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Get child plugs
	//
	MPlug previousPlug = previousElement.child(RotationList::rotation, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MPlug newPlug = newElement.child(RotationList::rotation, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Check if source plug has incoming connections
	// If so, then move those connections back to the previous plug
	//
	status = Maxformations::breakConnections(previousPlug, true, false);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::transferConnections(rotatePlug, previousPlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Check if new plug has incoming connections
	// If so, then move those connections to the translate plug
	//
	status = Maxformations::transferValues(newPlug, rotatePlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::transferConnections(newPlug, rotatePlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Connect translate plug to new plug
	//
	status = Maxformations::connectPlugs(rotatePlug, newPlug, true);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Update internal tracker
	//
	this->previousIndex = this->activeIndex;

	return MS::kSuccess;

};


MQuaternion RotationList::sum(MArrayDataHandle& handle, const unsigned int active, const bool normalizeWeights, MStatus* status)
/**
Returns the weighted average from the supplied array data handle.

@param handle: The array data handle to average from.
@param active: The index to average up to.
@param normalizeWeights: Determines if weights should be normalized.
@param status: Returns status.
@return: Weighted average vector.
*/
{

	// Check if active index is within range
	//
	unsigned int numElements = handle.elementCount(status);
	CHECK_MSTATUS_AND_RETURN(*status, MQuaternion::identity);

	if (!(0u <= active < numElements))
	{

		return MQuaternion::identity;

	}

	// Collect rotation entries
	//
	unsigned int numItems = active + 1;
	std::vector<RotationListItem> items = std::vector<RotationListItem>(numItems);

	MDataHandle elementHandle, nameHandle, weightHandle, absoluteHandle, axisOrderHandle, rotationHandle, rotationXHandle, rotationYHandle, rotationZHandle;
	MString name;
	float weight;
	bool absolute;
	Maxformations::AxisOrder axisOrder;
	double rotationX, rotationY, rotationZ;

	for (unsigned int i = 0; i < active; i++)
	{

		// Jump to array element
		//
		*status = handle.jumpToElement(i);
		CHECK_MSTATUS_AND_RETURN(*status, MQuaternion::identity)

		elementHandle = handle.inputValue(status);
		CHECK_MSTATUS_AND_RETURN(*status, MQuaternion::identity);

		// Get element data handles
		//
		nameHandle = elementHandle.child(RotationList::name);
		weightHandle = elementHandle.child(RotationList::weight);
		absoluteHandle = elementHandle.child(RotationList::absolute);
		axisOrderHandle = elementHandle.child(RotationList::axisOrder);
		rotationHandle = elementHandle.child(RotationList::rotation);
		rotationXHandle = rotationHandle.child(RotationList::x_rotation);
		rotationYHandle = rotationHandle.child(RotationList::y_rotation);
		rotationZHandle = rotationHandle.child(RotationList::z_rotation);

		// Get values from handles
		//
		name = nameHandle.asString();
		weight = weightHandle.asFloat();
		absolute = absoluteHandle.asBool();
		axisOrder = Maxformations::AxisOrder(axisOrderHandle.asShort());
		rotationX = rotationXHandle.asAngle().asRadians();
		rotationY = rotationYHandle.asAngle().asRadians();
		rotationZ = rotationZHandle.asAngle().asRadians();

		// Assign item to array
		//
		items[i] = RotationListItem{ name, weight, absolute, axisOrder, MVector(rotationX, rotationY, rotationZ) };

	}

	return RotationList::sum(items, normalizeWeights);

};


MQuaternion RotationList::sum(MArrayDataHandle& handle, const bool normalizeWeights, MStatus* status)
/**
Returns the weighted average from the supplied array data handle.

@param handle: The array data handle to average from.
@param normalizeWeights: Determines if weights should be normalized.
@param status: Returns status.
@return: Weighted average vector.
*/
{

	// Check if there are enough elements
	//
	unsigned int elementCount = handle.elementCount();
	unsigned int active = elementCount - 1;

	if (0u <= active < elementCount)
	{

		return RotationList::sum(handle, (elementCount - 1), normalizeWeights, status);

	}
	else
	{

		return MQuaternion::identity;

	}

};


MQuaternion RotationList::sum(std::vector<RotationListItem>& items, const bool normalizeWeights)
/**
Returns the weighted average of the supplied rotation items.

@param items: The rotation items to average.
@param normalizeWeights: Determines if weights should be normalized.
@return: Weighted average quaternion.
*/
{

	// Evaluate item count
	//
	unsigned long itemCount = items.size();
	MQuaternion average = MQuaternion(MQuaternion::identity);

	if (itemCount == 0)
	{

		return average;

	}

	// Check if weights should be normalized
	//
	if (normalizeWeights)
	{

		RotationList::normalize(items);

	}

	// Calculate weighted average
	//
	MQuaternion rotation;

	for (RotationListItem item : items)
	{

		// Evaluate which method to use
		//
		rotation = Maxformations::createRotationMatrix(item.rotate, item.axisOrder);

		if (item.absolute)
		{

			average = Maxformations::slerp(average, rotation, item.weight);

		}
		else
		{
			
			average = Maxformations::slerp(MQuaternion::identity, rotation, item.weight) * average;

		}

	}

	return average;
	
};


void RotationList::normalize(std::vector<RotationListItem>& items)
/**
Normalizes the passed weights so that the total sum equals 1.0.

@param items: The items to normalize.
@return: void
*/
{

	// Get weight sum
	//
	unsigned long itemCount = items.size();
	float sum = 0.0;

	for (unsigned long i = 0; i < itemCount; i++)
	{

		sum += std::fabs(items[i].weight);

	}

	// Check for divide by zero errors!
	//
	if (sum == 0.0 || sum == 1.0)
	{

		return;

	}

	// Multiply weights by scale factor
	//
	float factor = 1.0 / sum;

	for (unsigned long i = 0; i < itemCount; i++)
	{

		items[i].weight *= factor;

	}

};


void* RotationList::creator() 
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: RotationList
*/
{

	return new RotationList();

};


MStatus RotationList::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{
	
	MStatus status;

	// Initialize function sets
	//
	MFnNumericAttribute fnNumericAttr;
	MFnTypedAttribute fnTypedAttr;
	MFnUnitAttribute fnUnitAttr;
	MFnEnumAttribute fnEnumAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnCompoundAttribute fnCompoundAttr;

	// Input attributes:
	// ".active" attribute
	//
	RotationList::active = fnNumericAttr.create("active", "a", MFnNumericData::kInt, 0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));

	// ".average" attribute
	//
	RotationList::average = fnNumericAttr.create("average", "avg", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::inputCategory));

	// ".name" attribute
	//
	RotationList::name = fnTypedAttr.create("name", "n", MFnData::kString, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(RotationList::listCategory));

	// ".weight" attribute
	//
	RotationList::weight = fnNumericAttr.create("weight", "w", MFnNumericData::kFloat, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setMin(-1.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::listCategory));

	// ".absolute" attribute
	//
	RotationList::absolute = fnNumericAttr.create("absolute", "abs", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::listCategory));

	// ".axisOrder" attribute
	//
	RotationList::axisOrder = fnEnumAttr.create("axisOrder", "ao", short(1), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 5));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 6));
	CHECK_MSTATUS(fnEnumAttr.addField("xyx", 7));
	CHECK_MSTATUS(fnEnumAttr.addField("yzy", 8));
	CHECK_MSTATUS(fnEnumAttr.addField("zxz", 9));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(RotationList::listCategory));

	// ".x_rotation" attribute
	//
	RotationList::x_rotation = fnNumericAttr.create("x_rotation", "xr", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::rotationCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::listCategory));

	// ".y_rotation" attribute
	//
	RotationList::y_rotation = fnNumericAttr.create("y_rotation", "yr", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::rotationCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::listCategory));

	// ".z_rotation" attribute
	//
	RotationList::z_rotation = fnNumericAttr.create("z_rotation", "zr", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::rotationCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::listCategory));

	// ".rotation" attribute
	//
	RotationList::rotation = fnNumericAttr.create("rotation", "r", RotationList::x_rotation, RotationList::y_rotation, RotationList::z_rotation, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::rotationCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::listCategory));

	// ".list" attribute
	//
	RotationList::list = fnCompoundAttr.create("list", "l", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnCompoundAttr.addChild(RotationList::name));
	CHECK_MSTATUS(fnCompoundAttr.addChild(RotationList::weight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(RotationList::absolute));
	CHECK_MSTATUS(fnCompoundAttr.addChild(RotationList::axisOrder));
	CHECK_MSTATUS(fnCompoundAttr.addChild(RotationList::rotation));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(RotationList::listCategory));

	// Output attributes:
	// ".valueX" attribute
	//
	RotationList::valueX = fnNumericAttr.create("valueX", "vx", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".valueY" attribute
	//
	RotationList::valueY = fnNumericAttr.create("valueY", "vy", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".valueZ" attribute
	//
	RotationList::valueZ = fnNumericAttr.create("valueZ", "vz", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".valueW" attribute
	//
	RotationList::valueW = fnNumericAttr.create("valueW", "vw", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".value" attribute
	//
	RotationList::value = fnNumericAttr.create("value", "v", RotationList::valueX, RotationList::valueY, RotationList::valueZ, RotationList::valueW, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".preValueX" attribute
	//
	RotationList::preValueX = fnNumericAttr.create("preValueX", "vx", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".preValueY" attribute
	//
	RotationList::preValueY = fnNumericAttr.create("preValueY", "vy", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".preValueZ" attribute
	//
	RotationList::preValueZ = fnNumericAttr.create("preValueZ", "vz", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".preValueW" attribute
	//
	RotationList::preValueW = fnNumericAttr.create("preValueW", "vw", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".preValue" attribute
	//
	RotationList::preValue = fnNumericAttr.create("preValue", "v", RotationList::preValueX, RotationList::preValueY, RotationList::preValueZ, RotationList::preValueW, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::outputCategory));

	// ".matrix" attribute
	//
	RotationList::matrix = fnMatrixAttr.create("matrix", "m", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(RotationList::outputCategory));

	// ".inverseMatrix" attribute
	//
	RotationList::inverseMatrix = fnMatrixAttr.create("inverseMatrix", "im", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(RotationList::outputCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::active));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::average));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::list));

	CHECK_MSTATUS(RotationList::addAttribute(RotationList::value));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::preValue));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::matrix));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::inverseMatrix));

	// Define attribute relationships
	//
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::active, RotationList::preValue));

	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::average, RotationList::value));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::weight, RotationList::value));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::absolute, RotationList::value));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::axisOrder, RotationList::value));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::rotation, RotationList::value));

	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::average, RotationList::matrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::weight, RotationList::matrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::absolute, RotationList::matrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::axisOrder, RotationList::matrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::rotation, RotationList::matrix));

	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::average, RotationList::inverseMatrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::weight, RotationList::inverseMatrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::absolute, RotationList::inverseMatrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::axisOrder, RotationList::inverseMatrix));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::rotation, RotationList::inverseMatrix));

	return status;

};