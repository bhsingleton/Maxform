//
// File: ScaleListNode.cpp
//
// Dependency Graph Node: scaleList
//
// Author: Benjamin H. Singleton
//

#include "ScaleList.h"

MObject		ScaleList::active;
MObject		ScaleList::average;
MObject		ScaleList::list;
MObject		ScaleList::name;
MObject		ScaleList::weight;
MObject		ScaleList::absolute;
MObject		ScaleList::scale;
MObject		ScaleList::x_scale;
MObject		ScaleList::y_scale;
MObject		ScaleList::z_scale;

MObject		ScaleList::value;
MObject		ScaleList::valueX;
MObject		ScaleList::valueY;
MObject		ScaleList::valueZ;
MObject		ScaleList::preValue;
MObject		ScaleList::preValueX;
MObject		ScaleList::preValueY;
MObject		ScaleList::preValueZ;
MObject		ScaleList::matrix;
MObject		ScaleList::inverseMatrix;

MString		ScaleList::inputCategory("Input");
MString		ScaleList::outputCategory("Output");
MString		ScaleList::listCategory("List");
MString		ScaleList::scaleCategory("Scale");
MString		ScaleList::preScaleCategory("PreScale");

MTypeId		ScaleList::id(0x0013b1c7);


ScaleList::ScaleList()
/**
Constructor.
*/
{
	
	this->prs = nullptr;
	this->previousIndex = 0;
	this->activeIndex = 0;

};


ScaleList::~ScaleList()
/**
Destructor.
*/
{
	
	this->prs = nullptr;

};


MStatus ScaleList::compute(const MPlug& plug, MDataBlock& data) 
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

	if (fnAttribute.hasCategory(ScaleList::outputCategory))
	{
		
		// Get input data handles
		//
		MDataHandle averageHandle = data.inputValue(ScaleList::average, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle listHandle = data.inputArrayValue(ScaleList::list, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		bool normalizeWeights = averageHandle.asBool();
		
		// Calculate weighted average
		//
		MVector scale = ScaleList::sum(listHandle, normalizeWeights, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MMatrix matrix = Maxformations::createScaleMatrix(scale);
		
		// Get output data handles
		//
		MDataHandle valueXHandle = data.outputValue(ScaleList::valueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueYHandle = data.outputValue(ScaleList::valueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle valueZHandle = data.outputValue(ScaleList::valueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle matrixHandle = data.outputValue(ScaleList::matrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle inverseMatrixHandle = data.outputValue(ScaleList::inverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		valueXHandle.setDouble(scale.x);
		valueXHandle.setClean();

		valueYHandle.setDouble(scale.y);
		valueYHandle.setClean();

		valueZHandle.setDouble(scale.z);
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
	else if (fnAttribute.hasCategory(ScaleList::preScaleCategory))
	{

		// Get input data handles
		//
		MDataHandle activeHandle = data.inputValue(ScaleList::active, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle averageHandle = data.inputValue(ScaleList::average, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle listHandle = data.inputArrayValue(ScaleList::list, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		unsigned int active = activeHandle.asShort();
		bool normalizeWeights = averageHandle.asBool();

		// Calculate weighted average
		//
		MVector scale = ScaleList::sum(listHandle, active - 1, normalizeWeights, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get output data handles
		//
		MDataHandle preValueXHandle = data.outputValue(ScaleList::preValueX, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preValueYHandle = data.outputValue(ScaleList::preValueY, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle preValueZHandle = data.outputValue(ScaleList::preValueZ, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
		preValueXHandle.setDouble(scale.x);
		preValueXHandle.setClean();

		preValueYHandle.setDouble(scale.y);
		preValueYHandle.setClean();

		preValueZHandle.setDouble(scale.z);
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


bool ScaleList::setInternalValue(const MPlug& plug, const MDataHandle& handle)
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
	bool isSceneLoading = Maxformations::isSceneLoading();

	if (plug == ScaleList::active && !isSceneLoading)
	{

		// Check if index is in range
		//
		MPlug listPlug = MPlug(this->thisMObject(), ScaleList::list);

		unsigned int numElements = listPlug.numElements();
		CHECK_MSTATUS_AND_RETURN(status, false);

		this->activeIndex = Maxformations::clamp(handle.asInt(), 0, (int)(numElements - 1));
		this->updateActiveController();

	}
	else if (plug == ScaleList::active && isSceneLoading)
	{

		// Cache active indices
		this->previousIndex = this->activeIndex = handle.asShort();

	}
	else;

	return MPxNode::setInternalValue(plug, handle);

};


MStatus ScaleList::updateActiveController()
/**
Updates the active controller.

@param scalePlug: The incoming scale plug.
@return: Status code.
*/
{

	MStatus status;

	// Redundancy check
	//
	Maxform* maxform = this->maxformPtr();
	bool isSceneLoading = Maxformations::isSceneLoading();

	if (this->previousIndex == this->activeIndex || maxform == nullptr || isSceneLoading)
	{

		return MS::kSuccess;  // Nothing to do here~!

	}

	// Transfer connections
	//
	this->pullController(this->previousIndex);
	this->pushController(this->activeIndex);

	MGlobal::displayInfo("Updated active scale controller!");
	this->previousIndex = this->activeIndex;

	return MS::kSuccess;

};


MStatus ScaleList::pullController(unsigned int index)
/**
Transfers any connections from the associated maxform back to the specified index.

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

	// Get scale plugs
	//
	MPlug scalePlug = MPlug(maxform->thisMObject(), Maxform::rotate);

	MPlug listPlug = MPlug(this->thisMObject(), ScaleList::list);
	MPlug controllerPlug = listPlug.elementByLogicalIndex(index).child(ScaleList::scale);

	// Transfer connections to controller
	//
	status = Maxformations::breakConnections(controllerPlug, true, false);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::transferConnections(scalePlug, controllerPlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus ScaleList::pushController(unsigned int index)
/**
Transfers any connections from the specified index to the associated maxform.

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

	// Get scale plugs
	//
	MPlug scalePlug = MPlug(maxform->thisMObject(), Maxform::scale);

	MPlug listPlug = MPlug(this->thisMObject(), ScaleList::list);
	MPlug controllerPlug = listPlug.elementByLogicalIndex(index).child(ScaleList::scale);

	// Update controller connections
	//
	status = Maxformations::transferValues(controllerPlug, scalePlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::transferConnections(controllerPlug, scalePlug);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = Maxformations::connectPlugs(scalePlug, controllerPlug, true);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;

};


MStatus ScaleList::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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

	bool isOutput = fnAttribute.hasCategory(ScaleList::outputCategory);

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


MStatus ScaleList::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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
		MPlug scalePlug = MPlug(this->prs->thisMObject(), PRS::scale);

		bool isPartiallyConnected = Maxformations::isPartiallyConnected(scalePlug, true, false, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (!isPartiallyConnected)
		{

			this->prs = nullptr;

		}

	}

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


MVector ScaleList::sum(MArrayDataHandle& handle, const unsigned int active, const bool normalizeWeights, MStatus* status)
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
	CHECK_MSTATUS_AND_RETURN(*status, MVector::one);

	if (!(0u <= active && active < numElements))
	{

		return MVector::one;

	}

	// Collect scale entries
	//
	unsigned int numItems = active + 1;
	std::vector<ScaleListItem> items = std::vector<ScaleListItem>(numItems);

	MDataHandle elementHandle, nameHandle, weightHandle, absoluteHandle, scaleHandle, scaleXHandle, scaleYHandle, scaleZHandle;
	MString name;
	float weight;
	bool absolute;
	double scaleX, scaleY, scaleZ;

	for (unsigned int i = 0; i < numItems; i++)
	{

		// Jump to array element
		//
		*status = handle.jumpToElement(i);
		CHECK_MSTATUS_AND_RETURN(*status, MVector::one)

		elementHandle = handle.inputValue(status);
		CHECK_MSTATUS_AND_RETURN(*status, MVector::one);

		// Get element data handles
		//
		nameHandle = elementHandle.child(ScaleList::name);
		weightHandle = elementHandle.child(ScaleList::weight);
		absoluteHandle = elementHandle.child(ScaleList::absolute);
		scaleHandle = elementHandle.child(ScaleList::scale);
		scaleXHandle = scaleHandle.child(ScaleList::x_scale);
		scaleYHandle = scaleHandle.child(ScaleList::y_scale);
		scaleZHandle = scaleHandle.child(ScaleList::z_scale);

		// Get values from handles
		//
		name = nameHandle.asString();
		weight = weightHandle.asFloat();
		absolute = absoluteHandle.asBool();
		scaleX = scaleXHandle.asDouble();
		scaleY = scaleYHandle.asDouble();
		scaleZ = scaleZHandle.asDouble();

		// Assign value to arrays
		//
		items[i] = ScaleListItem{ name, weight, absolute, MVector(scaleX, scaleY, scaleZ) };

	}

	return ScaleList::sum(items, normalizeWeights);

};


MVector ScaleList::sum(MArrayDataHandle& handle, const bool normalizeWeights, MStatus* status)
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

		return ScaleList::sum(handle, (elementCount - 1), normalizeWeights, status);

	}
	else
	{

		return MVector::one;

	}

};


MVector ScaleList::sum(std::vector<ScaleListItem>& items, const bool normalizeWeights)
/**
Returns the weighted average of the supplied scale items.

@param items: The scale items to average.
@return: Weighted average vector.
*/
{
	
	// Evaluate item count
	//
	unsigned long itemCount = items.size();
	MVector average = MVector(MVector::one);

	if (itemCount == 0)
	{

		return average;

	}
	
	// Check if weights should be normalized
	//
	if (normalizeWeights)
	{

		ScaleList::normalize(items);

	}

	// Calculate weighted average
	//
	MVector scale;

	for (ScaleListItem item : items)
	{
		
		// Evaluate which method to use
		//
		if (item.absolute)
		{

			average = Maxformations::lerp(average, item.scale, item.weight);

		}
		else if (abs(item.weight) > DBL_MIN)
		{

			scale = Maxformations::lerp(MVector::one, item.scale, item.weight);
			average.x *= scale.x;
			average.y *= scale.y;
			average.z *= scale.z;

		}
		else
		{

			continue;

		}

	}
	
	return average;
	
}


void ScaleList::normalize(std::vector<ScaleListItem>& items)
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


Maxform* ScaleList::maxformPtr()
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


void* ScaleList::creator() 
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: ScaleList
*/
{

	return new ScaleList();

};


MStatus ScaleList::initialize()
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
	ScaleList::active = fnNumericAttr.create("active", "a", MFnNumericData::kInt, 0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));

	// ".average" attribute
	//
	ScaleList::average = fnNumericAttr.create("average", "avg", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::inputCategory));

	// ".name" attribute
	//
	ScaleList::name = fnTypedAttr.create("name", "n", MFnData::kString, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnTypedAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnTypedAttr.addToCategory(ScaleList::listCategory));

	// ".weight" attribute
	//
	ScaleList::weight = fnNumericAttr.create("weight", "w", MFnNumericData::kFloat, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setMin(-1.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::listCategory));

	// ".absolute" attribute
	//
	ScaleList::absolute = fnNumericAttr.create("absolute", "abs", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::listCategory));

	// ".x_scale" attribute
	//
	ScaleList::x_scale = fnNumericAttr.create("x_scale", "xs", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::scaleCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::listCategory));

	// ".y_scale" attribute
	//
	ScaleList::y_scale = fnNumericAttr.create("y_scale", "ys",  MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::scaleCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::listCategory));

	// ".z_scale" attribute
	//
	ScaleList::z_scale = fnNumericAttr.create("z_scale", "zs",  MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::scaleCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::listCategory));

	// ".position" attribute
	//
	ScaleList::scale = fnNumericAttr.create("scale", "s", ScaleList::x_scale, ScaleList::y_scale, ScaleList::z_scale, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::scaleCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::listCategory));

	// ".list" attribute
	//
	ScaleList::list = fnCompoundAttr.create("list", "l", &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnCompoundAttr.addChild(ScaleList::name));
	CHECK_MSTATUS(fnCompoundAttr.addChild(ScaleList::weight));
	CHECK_MSTATUS(fnCompoundAttr.addChild(ScaleList::absolute));
	CHECK_MSTATUS(fnCompoundAttr.addChild(ScaleList::scale));
	CHECK_MSTATUS(fnCompoundAttr.setArray(true));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(ScaleList::inputCategory));
	CHECK_MSTATUS(fnCompoundAttr.addToCategory(ScaleList::listCategory));

	// Output attributes:
	// ".valueX" attribute
	//
	ScaleList::valueX = fnNumericAttr.create("valueX", "vx", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::outputCategory));

	// ".valueY" attribute
	//
	ScaleList::valueY = fnNumericAttr.create("valueY", "vy", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::outputCategory));

	// ".valueZ" attribute
	//
	ScaleList::valueZ = fnNumericAttr.create("valueZ", "vz", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::outputCategory));

	// ".value" attribute
	//
	ScaleList::value = fnNumericAttr.create("value", "v", ScaleList::valueX, ScaleList::valueY, ScaleList::valueZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::outputCategory));

	// ".preValueX" attribute
	//
	ScaleList::preValueX = fnNumericAttr.create("preValueX", "pvx", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::preScaleCategory));

	// ".preValueY" attribute
	//
	ScaleList::preValueY = fnNumericAttr.create("preValueY", "pvy", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::preScaleCategory));

	// ".preValueZ" attribute
	//
	ScaleList::preValueZ = fnNumericAttr.create("preValueZ", "pvz", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::preScaleCategory));

	// ".preValue" attribute
	//
	ScaleList::preValue = fnNumericAttr.create("preValue", "pv", ScaleList::preValueX, ScaleList::preValueY, ScaleList::preValueZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setWritable(false));
	CHECK_MSTATUS(fnNumericAttr.setStorable(false));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(ScaleList::preScaleCategory));

	// ".matrix" attribute
	//
	ScaleList::matrix = fnMatrixAttr.create("matrix", "m", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(ScaleList::outputCategory));

	// ".inverseMatrix" attribute
	//
	ScaleList::inverseMatrix = fnMatrixAttr.create("inverseMatrix", "im", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(ScaleList::outputCategory));


	// Add attributes to node
	//
	CHECK_MSTATUS(ScaleList::addAttribute(ScaleList::active));
	CHECK_MSTATUS(ScaleList::addAttribute(ScaleList::average));
	CHECK_MSTATUS(ScaleList::addAttribute(ScaleList::list));

	CHECK_MSTATUS(ScaleList::addAttribute(ScaleList::value));
	CHECK_MSTATUS(ScaleList::addAttribute(ScaleList::preValue));
	CHECK_MSTATUS(ScaleList::addAttribute(ScaleList::matrix));
	CHECK_MSTATUS(ScaleList::addAttribute(ScaleList::inverseMatrix));

	// Define attribute relationships
	//
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::active, ScaleList::preValue));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::average, ScaleList::preValue));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::weight, ScaleList::preValue));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::absolute, ScaleList::preValue));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::scale, ScaleList::preValue));

	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::average, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::weight, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::absolute, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::scale, ScaleList::value));

	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::average, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::weight, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::absolute, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::scale, ScaleList::value));

	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::average, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::weight, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::absolute, ScaleList::value));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::scale, ScaleList::value));

	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::average, ScaleList::matrix));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::weight, ScaleList::matrix));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::absolute, ScaleList::matrix));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::scale, ScaleList::matrix));

	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::average, ScaleList::inverseMatrix));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::weight, ScaleList::inverseMatrix));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::absolute, ScaleList::inverseMatrix));
	CHECK_MSTATUS(ScaleList::attributeAffects(ScaleList::scale, ScaleList::inverseMatrix));

	return status;

};