//
// File: RotationList.cpp
//
// Dependency Graph Node: rotationList
//
// Author: Benjamin H. Singleton
//

#include "RotationList.h"

MObject	RotationList::active;
MObject	RotationList::average;
MObject	RotationList::list;
MObject	RotationList::name;
MObject	RotationList::weight;
MObject	RotationList::absolute;
MObject	RotationList::axisOrder;
MObject	RotationList::rotation;
MObject	RotationList::x_rotation;
MObject	RotationList::y_rotation;
MObject	RotationList::z_rotation;

MObject	RotationList::preValue;
MObject	RotationList::preValueX;
MObject	RotationList::preValueY;
MObject	RotationList::preValueZ;
MObject	RotationList::matrix;
MObject	RotationList::inverseMatrix;

MString	RotationList::inputCategory("Input");
MString	RotationList::listCategory("List");
MString	RotationList::preValueCategory("PreValue");

MTypeId	RotationList::id(0x0013b1c6);


RotationList::RotationList() : RotationController() { this->previousIndex = -1; this->activeIndex = -1; };
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

	bool isValue = fnAttribute.hasCategory(RotationList::valueCategory);
	bool isPreValue = fnAttribute.hasCategory(RotationList::preValueCategory);

	if (isValue)
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
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MMatrix matrix = quat.asMatrix();
		MEulerRotation eulerAngles = Maxformations::matrixToEulerRotation(matrix, MEulerRotation::RotationOrder::kXYZ);

		// Get output data handles
		//
		MDataHandle valueXHandle = data.outputValue(RotationList::valueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueYHandle = data.outputValue(RotationList::valueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueZHandle = data.outputValue(RotationList::valueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle matrixHandle = data.outputValue(RotationList::matrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle inverseMatrixHandle = data.outputValue(RotationList::inverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Set output handle values
		//
		valueXHandle.setMAngle(MAngle(eulerAngles.x));
		valueXHandle.setClean();

		valueYHandle.setMAngle(MAngle(eulerAngles.y));
		valueYHandle.setClean();

		valueZHandle.setMAngle(MAngle(eulerAngles.z));
		valueZHandle.setClean();

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
	else if (isPreValue)
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

		MMatrix matrix = quat.asMatrix();
		MEulerRotation eulerAngles = Maxformations::matrixToEulerRotation(matrix, MEulerRotation::RotationOrder::kXYZ);

		// Get output data handles
		//
		MDataHandle preValueXHandle = data.outputValue(RotationList::preValueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preValueYHandle = data.outputValue(RotationList::preValueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preValueZHandle = data.outputValue(RotationList::preValueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Set output handle preValues
		//
		preValueXHandle.setMAngle(MAngle(eulerAngles.x));
		preValueXHandle.setClean();

		preValueYHandle.setMAngle(MAngle(eulerAngles.y));
		preValueYHandle.setClean();

		preValueZHandle.setMAngle(MAngle(eulerAngles.z));
		preValueZHandle.setClean();

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

	// Evaluate plug
	//
	if (plug == RotationList::active)
	{

		// Check is scene being loaded
		// 
		bool isSceneLoading = Maxformations::isSceneLoading();

		if (isSceneLoading)
		{

			// Cache active indices
			//
			this->previousIndex = this->activeIndex = handle.asShort();

		}
		else
		{

			// Update active controller
			//
			this->activeIndex = handle.asInt();
			this->updateActiveController();

		}


	}

	return MPxNode::setInternalValue(plug, handle);

};


void RotationList::dependentChanged(const MObject& otherNode)
/**
Notifies that a dependent of this node has changed.

@param node: The node that is dependent on this controller.
@return: Void.
*/
{
	
	this->updateActiveController();

};


MStatus RotationList::updateActiveController()
/**
Updates the active controller.

@param activeIndex: The new active index.
@return: Status code.
*/
{

	MStatus status;

	// Redundancy check
	//
	Maxform* maxform = this->getAssociatedTransform(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (this->previousIndex == this->activeIndex || maxform == nullptr)
	{

		return MS::kSuccess;  // Nothing to do here~!

	}

	// Transfer connections
	//
	MStatus pullStatus = this->pullController(this->previousIndex);
	MStatus pushStatus = this->pushController(this->activeIndex);

	if (pullStatus || pushStatus)
	{

		MGlobal::displayInfo("Updated active rotation controller!");
		this->previousIndex = this->activeIndex;

	}

	return MS::kSuccess;

};


MStatus RotationList::pullController(unsigned int index)
/**
Transfers any connections from the associated maxform back to the specified index.

@param index: The index to transfer connections to.
@return: Return status.
*/
{

	MStatus status;

	// Check if maxform exists
	//
	Maxform* maxform = this->getAssociatedTransform(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (maxform == nullptr)
	{

		return MS::kNotFound;

	}

	// Check if active index is in range
	//
	MPlug listPlug = MPlug(this->thisMObject(), RotationList::list);

	unsigned int numElements = listPlug.numElements(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (0 <= index && index < numElements)
	{

		// Get required plugs
		//
		MPlug rotatePlug = MPlug(maxform->thisMObject(), Maxform::rotate);
		MPlug rotateOrderPlug = MPlug(maxform->thisMObject(), Maxform::rotateOrder);

		MPlug listElement = listPlug.elementByLogicalIndex(index);
		MPlug rotationElement = listElement.child(RotationList::rotation);
		MPlug axisOrderElement = listElement.child(RotationList::axisOrder);

		// Pull rotation connections from maxform
		//
		status = Maxformations::breakConnections(rotationElement, true, false);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::transferConnections(rotatePlug, rotationElement);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Pull rotate-order connections from maxform
		//
		status = Maxformations::breakConnections(axisOrderElement, true, false);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::transferConnections(rotateOrderPlug, axisOrderElement);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return status;

	}
	else
	{

		return MS::kFailure;

	}

};


MStatus RotationList::pushController(unsigned int index)
/**
Transfers any connections from the specified index to the associated maxform.

@param index: The index to transfer connections from.
@return: Return status.
*/
{

	MStatus status;

	// Check if maxform exists
	//
	Maxform* maxform = this->getAssociatedTransform(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (maxform == nullptr)
	{

		return MS::kNotFound;

	}

	// Check if active index is in range
	//
	MPlug listPlug = MPlug(this->thisMObject(), RotationList::list);

	unsigned int numElements = listPlug.numElements(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (0 <= index && index < numElements)
	{

		// Get required plugs
		//
		MPlug rotatePlug = MPlug(maxform->thisMObject(), Maxform::rotate);
		MPlug rotateOrderPlug = MPlug(maxform->thisMObject(), Maxform::rotateOrder);

		MPlug listElement = listPlug.elementByLogicalIndex(index);
		MPlug rotationElement = listElement.child(RotationList::rotation);
		MPlug axisOrderElement = listElement.child(RotationList::axisOrder);

		// Push rotation connections to maxform
		//
		status = Maxformations::transferValues(rotationElement, rotatePlug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::transferConnections(rotationElement, rotatePlug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::connectPlugs(rotatePlug, rotationElement, true);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Push axis-order connections to maxform
		//
		status = Maxformations::transferValues(axisOrderElement, rotateOrderPlug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::transferConnections(axisOrderElement, rotateOrderPlug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::connectPlugs(rotateOrderPlug, axisOrderElement, true);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return status;

	}
	else
	{

		return MS::kNotFound;

	}

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

	if (!(0u <= active && active < numElements))
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
	MEulerRotation::RotationOrder axisOrder;
	double rotationX, rotationY, rotationZ;

	for (unsigned int i = 0; i < numItems; i++)
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
		weight = Maxformations::clamp(weightHandle.asFloat(), -1.0f, 1.0f);
		absolute = absoluteHandle.asBool();
		axisOrder = MEulerRotation::RotationOrder(axisOrderHandle.asShort());
		rotationX = rotationXHandle.asAngle().asRadians();
		rotationY = rotationYHandle.asAngle().asRadians();
		rotationZ = rotationZHandle.asAngle().asRadians();

		// Assign item to array
		//
		items[i] = RotationListItem{ name, weight, absolute, MEulerRotation(rotationX, rotationY, rotationZ, axisOrder) };

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

	if (0u <= active && active < elementCount)
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
		rotation = item.eulerRotation.asQuaternion();

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


bool RotationList::isAbstractClass() const
/**
Override this class to return true if this node is an abstract node.
An abstract node can only be used as a base class. It cannot be created using the 'createNode' command.

@return: True if the node is abstract.
*/
{

	return false;

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

	CHECK_MSTATUS(fnNumericAttr.setMin(0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));

	// ".average" attribute
	//
	RotationList::average = fnNumericAttr.create("average", "avg", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::inputCategory));

	// ".name" attribute
	//
	RotationList::name = fnTypedAttr.create("name", "n", MFnData::kString, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(RotationList::listCategory));

	// ".weight" attribute
	//
	RotationList::weight = fnNumericAttr.create("weight", "w", MFnNumericData::kFloat, 1.0f, &status);
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
	RotationList::axisOrder = fnEnumAttr.create("axisOrder", "ao", short(0), &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnEnumAttr.addField("xyz", 0));
	CHECK_MSTATUS(fnEnumAttr.addField("yzx", 1));
	CHECK_MSTATUS(fnEnumAttr.addField("zxy", 2));
	CHECK_MSTATUS(fnEnumAttr.addField("xzy", 3));
	CHECK_MSTATUS(fnEnumAttr.addField("yxz", 4));
	CHECK_MSTATUS(fnEnumAttr.addField("zyx", 5));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(RotationList::listCategory));

	// ".x_rotation" attribute
	//
	RotationList::x_rotation = fnUnitAttr.create("x_rotation", "xr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::listCategory));

	// ".y_rotation" attribute
	//
	RotationList::y_rotation = fnUnitAttr.create("y_rotation", "yr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::listCategory));

	// ".z_rotation" attribute
	//
	RotationList::z_rotation = fnUnitAttr.create("z_rotation", "zr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::listCategory));

	// ".rotation" attribute
	//
	RotationList::rotation = fnNumericAttr.create("rotation", "r", RotationList::x_rotation, RotationList::y_rotation, RotationList::z_rotation, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::inputCategory));
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
	// ".preValueX" attribute
	//
	RotationList::preValueX = fnUnitAttr.create("preValueX", "pvx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::preValueCategory));

	// ".preValueY" attribute
	//
	RotationList::preValueY = fnUnitAttr.create("preValueY", "pvy", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::preValueCategory));

	// ".preValueZ" attribute
	//
	RotationList::preValueZ = fnUnitAttr.create("preValueZ", "pvz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(RotationList::preValueCategory));

	// ".preValue" attribute
	//
	RotationList::preValue = fnNumericAttr.create("preValue", "pv", RotationList::preValueX, RotationList::preValueY, RotationList::preValueZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(RotationList::preValueCategory));

	// ".matrix" attribute
	//
	RotationList::matrix = fnMatrixAttr.create("matrix", "m", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(RotationList::valueCategory));

	// ".inverseMatrix" attribute
	//
	RotationList::inverseMatrix = fnMatrixAttr.create("inverseMatrix", "im", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(RotationList::valueCategory));

	// Inherit attribute from parent class
	//
	status = RotationList::inheritAttributesFrom("rotationController");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Add attributes to node
	//
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::active));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::average));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::list));

	CHECK_MSTATUS(RotationList::addAttribute(RotationList::preValue));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::matrix));
	CHECK_MSTATUS(RotationList::addAttribute(RotationList::inverseMatrix));

	// Define attribute relationships
	//
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::active, RotationList::preValue));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::average, RotationList::preValue));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::weight, RotationList::preValue));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::absolute, RotationList::preValue));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::axisOrder, RotationList::preValue));
	CHECK_MSTATUS(RotationList::attributeAffects(RotationList::rotation, RotationList::preValue));

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