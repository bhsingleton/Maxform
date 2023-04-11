//
// File: PositionListNode.cpp
//
// Dependency Graph Node: positionList
//
// Author: Benjamin H. Singleton
//

#include "PositionList.h"

MObject		PositionList::active;
MObject		PositionList::average;
MObject		PositionList::list;
MObject		PositionList::name;
MObject		PositionList::weight;
MObject		PositionList::absolute;
MObject		PositionList::position;
MObject		PositionList::x_position;
MObject		PositionList::y_position;
MObject		PositionList::z_position;

MObject		PositionList::value;
MObject		PositionList::valueX;
MObject		PositionList::valueY;
MObject		PositionList::valueZ;
MObject		PositionList::preValue;
MObject		PositionList::preValueX;
MObject		PositionList::preValueY;
MObject		PositionList::preValueZ;
MObject		PositionList::matrix;
MObject		PositionList::inverseMatrix;

MString		PositionList::inputCategory("Input");
MString		PositionList::outputCategory("Output");
MString		PositionList::listCategory("List");
MString		PositionList::positionCategory("Position");
MString		PositionList::prePositionCategory("PrePosition");

MTypeId		PositionList::id(0x0013b1c5);


PositionList::PositionList()
/**
Constructor.
*/
{
	
	this->prs = nullptr; 
	this->previousIndex = -1; 
	this->activeIndex = -1;

};


PositionList::~PositionList()
/**
Destructor.
*/
{

	this->prs = nullptr;

};


MStatus PositionList::compute(const MPlug& plug, MDataBlock& data) 
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

	// Evaluate requested plug
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (fnAttribute.hasCategory(PositionList::outputCategory))
	{
		
		// Get input data handles
		//
		MDataHandle averageHandle = data.inputValue(PositionList::average, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle listHandle = data.inputArrayValue(PositionList::list, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get input values
		//
		bool normalizeWeights = averageHandle.asBool();

		// Calculate weighted average
		//
		MVector position = PositionList::sum(listHandle, normalizeWeights, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MMatrix matrix = Maxformations::createPositionMatrix(position);
		
		// Get output data handles
		//
		MDataHandle valueXHandle = data.outputValue(PositionList::valueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueYHandle = data.outputValue(PositionList::valueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueZHandle = data.outputValue(PositionList::valueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle matrixHandle = data.outputValue(PositionList::matrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle inverseMatrixHandle = data.outputValue(PositionList::inverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		valueXHandle.setMDistance(MDistance(position.x, MDistance::kCentimeters));
		valueXHandle.setClean();
		
		valueYHandle.setMDistance(MDistance(position.y, MDistance::kCentimeters));
		valueYHandle.setClean();
		
		valueZHandle.setMDistance(MDistance(position.z, MDistance::kCentimeters));
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
	else if (fnAttribute.hasCategory(PositionList::prePositionCategory))
	{
		
		// Get input data handle
		//
		MDataHandle activeHandle = data.inputValue(PositionList::active, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle averageHandle = data.inputValue(PositionList::average, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle listHandle = data.inputArrayValue(PositionList::list, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get input values
		//
		unsigned int active = activeHandle.asShort();
		bool normalizeWeights = averageHandle.asBool();

		// Calculate weighted average
		//
		MVector position = PositionList::sum(listHandle, active - 1, normalizeWeights, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get output data handles
		//
		MDataHandle prePositionXHandle = data.outputValue(PositionList::preValueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle prePositionYHandle = data.outputValue(PositionList::preValueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle prePositionZHandle = data.outputValue(PositionList::preValueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		prePositionXHandle.setMDistance(MDistance(position.x, MDistance::kCentimeters));
		prePositionXHandle.setClean();

		prePositionYHandle.setMDistance(MDistance(position.y, MDistance::kCentimeters));
		prePositionYHandle.setClean();

		prePositionZHandle.setMDistance(MDistance(position.z, MDistance::kCentimeters));
		prePositionZHandle.setClean();

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


bool PositionList::setInternalValue(const MPlug& plug, const MDataHandle& handle)
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
	if (plug == PositionList::active)
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


MStatus PositionList::updateActiveController()
/**
Updates the active controller.

@param translatePlug: The new incoming translate plug.
@return: Status code.
*/
{

	// Redundancy check
	//
	Maxform* maxform = this->maxformPtr();

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

		MGlobal::displayInfo("Updated active position controller!");
		this->previousIndex = this->activeIndex;

	}

	return MS::kSuccess;

};


MStatus PositionList::pullController(unsigned int index)
/**
Transfers any connections from the associated maxform back to the specified list element.
A `MS::kNotFound` status will be returned if the index is not in range!

@param index: The index to transfer connections to.
@return: Return status.
*/
{

	MStatus status;

	// Check if maxform exists
	//
	Maxform* maxform = this->maxformPtr();

	if (maxform == nullptr)
	{

		return MS::kNotFound;

	}

	// Check if active index is in range
	//
	MPlug listPlug = MPlug(this->thisMObject(), PositionList::list);

	unsigned int numElements = listPlug.numElements(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (0 <= index && index < numElements)
	{

		// Get required plugs
		//
		MPlug translatePlug = MPlug(maxform->thisMObject(), Maxform::translate);

		MPlug listElement = listPlug.elementByLogicalIndex(index);
		MPlug positionElement = listElement.child(PositionList::position);

		// Pull position connections from maxform
		//
		status = Maxformations::breakConnections(positionElement, true, false);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::transferConnections(translatePlug, positionElement);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return status;

	}
	else
	{

		return MS::kNotFound;

	}

};


MStatus PositionList::pushController(unsigned int index)
/**
Transfers any connections from the specified list element to the associated maxform.
A `MS::kNotFound` status will be returned if the index is not in range!

@param index: The index to transfer connections from.
@return: Return status.
*/
{

	MStatus status;

	// Check if maxform exists
	//
	Maxform* maxform = this->maxformPtr();

	if (maxform == nullptr)
	{

		return MS::kNotFound;

	}

	// Check if active index is in range
	//
	MPlug listPlug = MPlug(this->thisMObject(), PositionList::list);

	unsigned int numElements = listPlug.numElements(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (0 <= index && index < numElements)
	{

		// Get required plugs
		//
		MPlug translatePlug = MPlug(maxform->thisMObject(), Maxform::translate);

		MPlug listElement = listPlug.elementByLogicalIndex(index);
		MPlug positionElement = listElement.child(PositionList::position);

		// Push position connections to maxform
		//
		status = Maxformations::transferValues(positionElement, translatePlug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::transferConnections(positionElement, translatePlug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = Maxformations::connectPlugs(translatePlug, positionElement, true);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return status;

	}
	else
	{

		return MS::kNotFound;

	}

};


MStatus PositionList::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
/**
This method gets called when connections are made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection or if you want maya to process the connection as well.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@return: Return status.
*/
{

	MStatus status;

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	bool isOutput = fnAttribute.hasCategory(PositionList::outputCategory);

	if ((isOutput && asSrc) && this->prs == nullptr)
	{

		// Inspect other node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId otherId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (otherId == PRS::id)
		{

			this->prs = static_cast<PRS*>(fnDependNode.userNode(&status));
			CHECK_MSTATUS_AND_RETURN_IT(status);

			this->updateActiveController();

		}

	}

	return MPxNode::connectionMade(plug, otherPlug, asSrc);

};


MStatus PositionList::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
/**
This method gets called when connections are made to attributes of this node.
You should return kUnknownParameter to specify that maya should handle this connection or if you want maya to process the connection as well.

@param plug: Attribute on this node.
@param otherPlug: Attribute on the other node.
@param asSrc: Is this plug a source of the connection.
@return: Return status.
*/
{

	MStatus status;

	// Inspect plug attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	bool isOutput = fnAttribute.hasCategory(PRS::valueCategory);

	if ((isOutput && asSrc) && this->prs != nullptr)
	{

		// Check if plug is still partially connected
		//
		MPlug positionPlug = MPlug(this->prs->thisMObject(), PRS::position);

		bool isPartiallyConnected = Maxformations::isPartiallyConnected(positionPlug, true, false, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (!isPartiallyConnected)
		{

			this->prs = nullptr;

		}
		
	}

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


MVector PositionList::sum(MArrayDataHandle& handle, const unsigned int active, const bool normalizeWeights, MStatus* status)
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
	CHECK_MSTATUS_AND_RETURN(*status, MVector::zero);

	if (!(0u <= active && active < numElements))
	{

		return MVector::zero;

	}

	// Collect position entries
	//
	unsigned int numItems = active + 1;
	std::vector<PositionListItem> items = std::vector<PositionListItem>(numItems);

	MDataHandle elementHandle, nameHandle, weightHandle, absoluteHandle, positionhandle, positionXHandle, positionYHandle, positionZHandle;
	MString name;
	float weight;
	bool absolute;
	double positionX, positionY, positionZ;

	for (unsigned int i = 0; i < numItems; i++)
	{

		// Jump to array element
		//
		*status = handle.jumpToElement(i);
		CHECK_MSTATUS_AND_RETURN(*status, MVector::zero);

		elementHandle = handle.inputValue(status);
		CHECK_MSTATUS_AND_RETURN(*status, MVector::zero);

		// Get element data handles
		//
		nameHandle = elementHandle.child(PositionList::name);
		weightHandle = elementHandle.child(PositionList::weight);
		absoluteHandle = elementHandle.child(PositionList::absolute);
		positionhandle = elementHandle.child(PositionList::position);
		positionXHandle = positionhandle.child(PositionList::x_position);
		positionYHandle = positionhandle.child(PositionList::y_position);
		positionZHandle = positionhandle.child(PositionList::z_position);

		// Get values from handles
		//
		name = nameHandle.asString();
		weight = Maxformations::clamp(weightHandle.asFloat(), -1.0f, 1.0f);
		absolute = absoluteHandle.asBool();
		positionX = positionXHandle.asDistance().asCentimeters();
		positionY = positionYHandle.asDistance().asCentimeters();
		positionZ = positionZHandle.asDistance().asCentimeters();

		// Assign value to arrays
		//
		items[i] = PositionListItem{ name, weight, absolute, MVector(positionX, positionY, positionZ) };

	}

	return PositionList::sum(items, normalizeWeights);

};


MVector PositionList::sum(MArrayDataHandle& handle, const bool normalizeWeights, MStatus* status)
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

		return PositionList::sum(handle, active, normalizeWeights, status);

	}
	else
	{

		return MVector::zero;

	}

};


MVector PositionList::sum(std::vector<PositionListItem>& items, const bool normalizeWeights)
/**
Returns the weighted average of the supplied position items.

@param items: The position items to average.
@param normalizeWeights: Determines if weights should be normalized.
@return: Weighted average vector.
*/
{
	
	// Evaluate item count
	//
	unsigned long itemCount = items.size();
	MVector average = MVector(MVector::zero);

	if (itemCount == 0)
	{

		return average;

	}

	// Check if weights should be normalized
	//
	if (normalizeWeights)
	{

		PositionList::normalize(items);

	}
	
	// Calculate weighted average
	//
	for (PositionListItem item : items)
	{

		// Evaluate which method to use
		//
		if (item.absolute)
		{

			average = Maxformations::lerp(average, item.translate, item.weight);

		}
		else
		{

			average += (item.translate * item.weight);

		}
		
	}
	
	return average;
	
}


void PositionList::normalize(std::vector<PositionListItem>& items)
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


Maxform* PositionList::maxformPtr()
/**
Returns the maxform node associated with this list controller.
If no maxform node exists then a null pointer is returned instead!

@return: Maxform pointer.
*/
{

	if (this->prs != nullptr)
	{

		return this->prs->maxformPtr();

	}
	else
	{

		return nullptr;

	}

}


void* PositionList::creator() 
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: PositionList
*/
{

	return new PositionList();

};


MStatus PositionList::initialize()
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
	MFnMatrixAttribute fnMatrixAttr;
	MFnCompoundAttribute fnCompoundAttr;

	// Input attributes:
	// ".active" attribute
	//
	PositionList::active = fnNumericAttr.create("active", "a", MFnNumericData::kInt, 0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));

	// ".average" attribute
	//
	PositionList::average = fnNumericAttr.create("average", "avg", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::inputCategory));

	// ".name" attribute
	//
	PositionList::name = fnTypedAttr.create("name", "n", MFnData::kString, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(PositionList::listCategory));

	// ".weight" attribute
	//
	PositionList::weight = fnNumericAttr.create("weight", "w", MFnNumericData::kFloat, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setMin(-1.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::listCategory));

	// ".absolute" attribute
	//
	PositionList::absolute = fnNumericAttr.create("absolute", "abs", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::listCategory));

	// ".x_position" attribute
	//
	PositionList::x_position = fnUnitAttr.create("x_position", "xp", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::positionCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::listCategory));

	// ".y_position" attribute
	//
	PositionList::y_position = fnUnitAttr.create("y_position", "yp",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::positionCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::listCategory));

	// ".z_position" attribute
	//
	PositionList::z_position = fnUnitAttr.create("z_position", "zp",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::positionCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::listCategory));

	// ".translate" attribute
	//
	PositionList::position = fnNumericAttr.create("position", "p", PositionList::x_position, PositionList::y_position, PositionList::z_position, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::positionCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::listCategory));

	// ".list" attribute
	//
	PositionList::list = fnCompoundAttr.create("list", "l", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionList::name));
	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionList::weight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionList::absolute));
	CHECK_MSTATUS(fnCompoundAttr.addChild(PositionList::position));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(PositionList::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(PositionList::listCategory));

	// Output attributes:
	// ".valueX" attribute
	//
	PositionList::valueX = fnUnitAttr.create("valueX", "vx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::outputCategory));

	// ".valueY" attribute
	//
	PositionList::valueY = fnUnitAttr.create("valueY", "vy", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::outputCategory));

	// ".valueZ" attribute
	//
	PositionList::valueZ = fnUnitAttr.create("valueZ", "vz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::outputCategory));

	// ".value" attribute
	//
	PositionList::value = fnNumericAttr.create("value", "v", PositionList::valueX, PositionList::valueY, PositionList::valueZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::outputCategory));

	// ".preValueX" attribute
	//
	PositionList::preValueX = fnUnitAttr.create("preValueX", "pvx", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::prePositionCategory));

	// ".preValueY" attribute
	//
	PositionList::preValueY = fnUnitAttr.create("preValueY", "pvy", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::prePositionCategory));

	// ".preValueZ" attribute
	//
	PositionList::preValueZ = fnUnitAttr.create("preValueZ", "pvz", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setWritable(false));
	CHECK_MSTATUS(fnUnitAttr.setStorable(false));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PositionList::prePositionCategory));

	// ".preValue" attribute
	//
	PositionList::preValue = fnNumericAttr.create("preValue", "pv", PositionList::preValueX, PositionList::preValueY, PositionList::preValueZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PositionList::prePositionCategory));

	// ".matrix" attribute
	//
	PositionList::matrix = fnMatrixAttr.create("matrix", "m", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionList::outputCategory));

	// ".inverseMatrix" attribute
	//
	PositionList::inverseMatrix = fnMatrixAttr.create("inverseMatrix", "im", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PositionList::outputCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(PositionList::addAttribute(PositionList::active));
	CHECK_MSTATUS(PositionList::addAttribute(PositionList::average));
	CHECK_MSTATUS(PositionList::addAttribute(PositionList::list));

	CHECK_MSTATUS(PositionList::addAttribute(PositionList::value));
	CHECK_MSTATUS(PositionList::addAttribute(PositionList::preValue));
	CHECK_MSTATUS(PositionList::addAttribute(PositionList::matrix));
	CHECK_MSTATUS(PositionList::addAttribute(PositionList::inverseMatrix));

	// Define attribute relationships
	//
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::active, PositionList::preValue));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::average, PositionList::preValue));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::weight, PositionList::preValue));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::absolute, PositionList::preValue));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::position, PositionList::preValue));

	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::average, PositionList::value));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::weight, PositionList::value));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::absolute, PositionList::value));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::position, PositionList::value));

	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::average, PositionList::matrix));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::weight, PositionList::matrix));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::absolute, PositionList::matrix));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::position, PositionList::matrix));

	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::average, PositionList::inverseMatrix));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::weight, PositionList::inverseMatrix));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::absolute, PositionList::inverseMatrix));
	CHECK_MSTATUS(PositionList::attributeAffects(PositionList::position, PositionList::inverseMatrix));

	return status;

};