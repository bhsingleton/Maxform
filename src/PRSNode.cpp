//
// File: PRSNode.cpp
//
// Dependency Graph Node: prs
//
// Author: Benjamin H. Singleton
//

#include "PRSNode.h"

MObject		PRS::position;
MObject		PRS::x_position;
MObject		PRS::y_position;
MObject		PRS::z_position;
MObject		PRS::rotation;
MObject		PRS::x_rotation;
MObject		PRS::y_rotation;
MObject		PRS::z_rotation;
MObject		PRS::w_rotation;
MObject		PRS::axisOrder;
MObject		PRS::euler_rotation;
MObject		PRS::euler_x_rotation;
MObject		PRS::euler_y_rotation;
MObject		PRS::euler_z_rotation;
MObject		PRS::scale;
MObject		PRS::x_scale;
MObject		PRS::y_scale;
MObject		PRS::z_scale;

MObject		PRS::matrix;
MObject		PRS::inverseMatrix;

MString		PRS::inputCategory("Input");
MString		PRS::outputCategory("Output");
MString		PRS::positionCategory("Position");
MString		PRS::rotationCategory("Rotation");
MString		PRS::eulerRotationCategory("EulerRotation");
MString		PRS::scaleCategory("Scale");

MTypeId		PRS::id(0x0013b1cb);


PRS::PRS() 
{ 
	
	this->maxform = nullptr; 
	this->positionList = nullptr; 
	this->rotationList = nullptr; 
	this->scaleList = nullptr; 

	this->activePositionChangedId = NULL;
	this->activeRotationChangedId = NULL;
	this->activeScaleChangedId = NULL;

};


PRS::~PRS() 
{ 
	
	this->maxform = nullptr; 
	this->positionList = nullptr; 
	this->rotationList = nullptr; 
	this->scaleList = nullptr; 

	this->activePositionChangedId = NULL;
	this->activeRotationChangedId = NULL;
	this->activeScaleChangedId = NULL;

};


MStatus PRS::compute(const MPlug& plug, MDataBlock& data) 
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

	if (fnAttribute.hasCategory(PRS::outputCategory))
	{
		
		// Get input data handles
		//
		MDataHandle positionHandle = data.inputValue(PRS::position, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		MDataHandle xPositionHandle = positionHandle.child(PRS::x_position);
		MDataHandle yPositionHandle = positionHandle.child(PRS::y_position);
		MDataHandle zPositionHandle = positionHandle.child(PRS::z_position);
		
		MDataHandle rotationHandle = data.inputValue(PRS::rotation, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		MDataHandle xRotationHandle = rotationHandle.child(PRS::x_rotation);
		MDataHandle yRotationHandle = rotationHandle.child(PRS::y_rotation);
		MDataHandle zRotationHandle = rotationHandle.child(PRS::z_rotation);
		MDataHandle wRotationHandle = rotationHandle.child(PRS::w_rotation);

		MDataHandle axisOrderHandle = data.inputValue(PRS::axisOrder, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle eulerRotationHandle = data.inputValue(PRS::euler_rotation, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle xEulerRotationHandle = eulerRotationHandle.child(PRS::euler_x_rotation);
		MDataHandle yEulerRotationHandle = eulerRotationHandle.child(PRS::euler_y_rotation);
		MDataHandle zEulerRotationHandle = eulerRotationHandle.child(PRS::euler_z_rotation);

		MDataHandle scaleHandle = data.inputValue(PRS::scale, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		MDataHandle xScaleHandle = scaleHandle.child(PRS::x_scale);
		MDataHandle yScaleHandle = scaleHandle.child(PRS::y_scale);
		MDataHandle zScaleHandle = scaleHandle.child(PRS::z_scale);
		
		// Get position value
		//
		double xPosition = xPositionHandle.asDistance().asCentimeters();
		double yPosition = yPositionHandle.asDistance().asCentimeters();
		double zPosition = zPositionHandle.asDistance().asCentimeters();

		MVector position = MVector(xPosition, yPosition, zPosition);
		
		// Get rotation value
		//
		double xRotation = xRotationHandle.asDouble();
		double yRotation = yRotationHandle.asDouble();
		double zRotation = zRotationHandle.asDouble();
		double wRotation = wRotationHandle.asDouble();

		MQuaternion rotation = MQuaternion(xRotation, yRotation, zRotation, wRotation);

		// Get euler rotation value
		//
		double xEulerRotation = xEulerRotationHandle.asAngle().asRadians();
		double yEulerRotation = yEulerRotationHandle.asAngle().asRadians();
		double zEulerRotation = zEulerRotationHandle.asAngle().asRadians();

		Maxformations::AxisOrder axisOrder = Maxformations::AxisOrder(axisOrderHandle.asShort());
		MVector eulerRotation = MVector(xEulerRotation, yEulerRotation, zEulerRotation);

		// Get scale value
		//
		double xScale = xScaleHandle.asDouble();
		double yScale = yScaleHandle.asDouble();
		double zScale = zScaleHandle.asDouble();

		MVector scale = MVector(xScale, yScale, zScale);
		
		// Compose transform matrix
		//
		MMatrix positionMatrix = Maxformations::createPositionMatrix(position);
		MMatrix rotationMatrix = rotation.asMatrix();
		MMatrix eulerRotationMatrix = Maxformations::createRotationMatrix(eulerRotation, axisOrder);
		MMatrix scaleMatrix = Maxformations::createScaleMatrix(scale);

		MMatrix matrix = scaleMatrix * (rotationMatrix * eulerRotationMatrix) * positionMatrix;
		
		// Get output data handles
		//
		MDataHandle matrixHandle = data.outputValue(PRS::matrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle inverseMatrixHandle = data.outputValue(PRS::inverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Update output data handles
		//
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
	else
	{

		return MS::kUnknownParameter;

	}

};


void activePositionChanged(MNodeMessage::AttributeMessage message, MPlug& plug, MPlug& otherPlug, void* clientData)
/**
Callback to whenever the active index changes on the associated position-list.

@param message: Type of message which caused the function to be called.
@param plug: First plug. Meaning depends upon the specific message which invoked the callback.
@param otherPlug: Second plug. Meaning depends upon the specific message which invoked the callback.
@param clientData: Pointer to user-defined data supplied when the callback was registered.
@return: void.
*/
{

	MObject attribute = plug.attribute();

	if (attribute == PositionList::active && message & MNodeMessage::kAttributeSet && clientData != nullptr)
	{

		PRS* prs = static_cast<PRS*>(clientData);
		prs->updateActivePosition();

	}

};


void activeRotationChanged(MNodeMessage::AttributeMessage message, MPlug& plug, MPlug& otherPlug, void* clientData)
/**
Callback to whenever the active index changes on the associated rotation-list.

@param message: Type of message which caused the function to be called.
@param plug: First plug. Meaning depends upon the specific message which invoked the callback.
@param otherPlug: Second plug. Meaning depends upon the specific message which invoked the callback.
@param clientData: Pointer to user-defined data supplied when the callback was registered.
@return: void.
*/
{

	MObject attribute = plug.attribute();

	if (attribute == RotationList::active && message & MNodeMessage::kAttributeSet && clientData != nullptr)
	{

		PRS* prs = static_cast<PRS*>(clientData);
		prs->updateActiveRotation();

	}

};


void activeScaleChanged(MNodeMessage::AttributeMessage message, MPlug& plug, MPlug& otherPlug, void* clientData)
/**
Callback to whenever the active index changes on the associated scale-list.

@param message: Type of message which caused the function to be called.
@param plug: First plug. Meaning depends upon the specific message which invoked the callback.
@param otherPlug: Second plug. Meaning depends upon the specific message which invoked the callback.
@param clientData: Pointer to user-defined data supplied when the callback was registered.
@return: void.
*/
{

	MObject attribute = plug.attribute();

	if (attribute == ScaleList::active && message & MNodeMessage::kAttributeSet && clientData != nullptr)
	{

		PRS* prs = static_cast<PRS*>(clientData);
		prs->updateActiveScale();

	}

};


MStatus PRS::connectionMade(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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

	bool isPosition = fnAttribute.hasCategory(PRS::positionCategory);
	bool isRotation = fnAttribute.hasCategory(PRS::rotationCategory);
	bool isScale = fnAttribute.hasCategory(PRS::scaleCategory);

	if ((attribute == PRS::matrix && asSrc) && this->maxform == nullptr)
	{

		// Inspect other node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId otherId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (otherId == Maxform::id)
		{

			this->maxform = static_cast<Maxform*>(fnDependNode.userNode());

		}

	}
	else if ((isPosition && !asSrc) && this->positionList == nullptr)
	{

		// Inspect other node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId otherId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (otherId == PositionList::id)
		{

			this->positionList = static_cast<PositionList*>(fnDependNode.userNode());

			this->activePositionChangedId = MNodeMessage::addAttributeChangedCallback(otherNode, activePositionChanged, this, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else if ((isRotation && !asSrc) && this->rotationList == nullptr)
	{

		// Inspect other node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId otherId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (otherId == RotationList::id)
		{

			this->rotationList = static_cast<RotationList*>(fnDependNode.userNode());

			this->activeRotationChangedId = MNodeMessage::addAttributeChangedCallback(otherNode, activeRotationChanged, this, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else if ((isScale && !asSrc) && this->scaleList == nullptr)
	{

		// Inspect other node
		//
		MObject otherNode = otherPlug.node(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDependNode(otherNode, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MTypeId otherId = fnDependNode.typeId(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (otherId == ScaleList::id)
		{

			this->scaleList = static_cast<ScaleList*>(fnDependNode.userNode());

			this->activeScaleChangedId = MNodeMessage::addAttributeChangedCallback(otherNode, activeScaleChanged, this, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else;

	return MPxNode::connectionMade(plug, otherPlug, asSrc);

};


MStatus PRS::connectionBroken(const MPlug& plug, const MPlug& otherPlug, bool asSrc)
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

	bool isPosition = fnAttribute.hasCategory(PRS::positionCategory);
	bool isRotation = fnAttribute.hasCategory(PRS::rotationCategory);
	bool isScale = fnAttribute.hasCategory(PRS::scaleCategory);
	
	if ((attribute == PRS::matrix && asSrc) && this->maxform != nullptr)
	{

		// Remove pointer to maxform
		//
		this->maxform = nullptr;

	}
	else if ((isPosition && !asSrc) && this->positionList != nullptr)
	{

		// Check if plug is still partially connected
		//
		MPlug positionPlug = MPlug(this->thisMObject(), PRS::position);

		bool isPartiallyConnected = Maxformations::isPartiallyConnected(positionPlug, true, false, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (!isPartiallyConnected)
		{

			this->positionList = nullptr;

			status = MNodeMessage::removeCallback(this->activePositionChangedId);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else if ((isRotation && !asSrc) && this->rotationList != nullptr)
	{

		// Check if plug is still partially connected
		//
		MPlug rotationPlug = MPlug(this->thisMObject(), PRS::rotation);

		bool isPartiallyConnected = Maxformations::isPartiallyConnected(rotationPlug, true, false, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (!isPartiallyConnected)
		{

			this->rotationList = nullptr;

			status = MNodeMessage::removeCallback(this->activeRotationChangedId);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else if ((isScale && !asSrc) && this->scaleList != nullptr)
	{

		// Check if plug is still partially connected
		//
		MPlug scalePlug = MPlug(this->thisMObject(), PRS::scale);

		bool isPartiallyConnected = Maxformations::isPartiallyConnected(scalePlug, true, false, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (!isPartiallyConnected)
		{

			this->scaleList = nullptr;

			status = MNodeMessage::removeCallback(this->activeScaleChangedId);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

	}
	else;

	return MPxNode::connectionBroken(plug, otherPlug, asSrc);

};


void PRS::updateActivePosition()
/**
Updates the active position controller.
This operation is only reserved for list controllers!

@return: void.
*/
{

	bool isSceneLoading = Maxformations::isSceneLoading();

	if ((this->positionList != nullptr && this->maxform != nullptr) && !isSceneLoading)
	{

		MPlug translatePlug = MPlug(this->maxform->thisMObject(), Maxform::translate);
		this->positionList->updateActiveController(translatePlug);

	}

};


void PRS::updateActiveRotation()
/**
Updates the active rotation controller.
This operation is only reserved for list controllers!

@return: void.
*/
{

	bool isSceneLoading = Maxformations::isSceneLoading();

	if ((this->rotationList != nullptr && this->maxform != nullptr) && !isSceneLoading)
	{

		MPlug rotatePlug = MPlug(this->maxform->thisMObject(), Maxform::rotate);
		this->rotationList->updateActiveController(rotatePlug);

	}

};


void PRS::updateActiveScale()
/**
Updates the active scale controller.
This operation is only reserved for list controllers!

@return: void.
*/
{

	bool isSceneLoading = Maxformations::isSceneLoading();

	if ((this->scaleList != nullptr && this->maxform != nullptr) && !isSceneLoading)
	{

		MPlug scalePlug = MPlug(this->maxform->thisMObject(), Maxform::scale);
		this->scaleList->updateActiveController(scalePlug);

	}

};


void PRS::updateMaxformPtr(Maxform* maxform)
/**
Updates the internal maxform node pointer.
This function is intended for use by IK-Chain controllers!

@param maxform: The new maxform pointer.
@return: void
*/
{

	this->maxform = maxform;

};


void* PRS::creator() 
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: PRS
*/
{

	return new PRS();

};


MStatus PRS::initialize()
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
	MFnUnitAttribute fnUnitAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnEnumAttribute fnEnumAttr;
	MFnCompoundAttribute fnCompoundAttr;

	// Input attributes:
	// ".x_position" attribute
	//
	PRS::x_position = fnUnitAttr.create("x_position", "xp", MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::positionCategory));

	// ".y_position" attribute
	//
	PRS::y_position = fnUnitAttr.create("y_position", "yp",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::positionCategory));
	
	// ".z_position" attribute
	//
	PRS::z_position = fnUnitAttr.create("z_position", "zp",  MFnUnitAttribute::kDistance, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));

	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::positionCategory));
	
	// ".position" attribute
	//
	PRS::position = fnNumericAttr.create("position", "p", PRS::x_position, PRS::y_position, PRS::z_position, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::positionCategory));

	// ".x_rotation" attribute
	//
	PRS::x_rotation = fnNumericAttr.create("x_rotation", "xr", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::rotationCategory));

	// ".y_rotation" attribute
	//
	PRS::y_rotation = fnNumericAttr.create("y_rotation", "yr", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::rotationCategory));

	// ".z_rotation" attribute
	//
	PRS::z_rotation = fnNumericAttr.create("z_rotation", "zr", MFnNumericData::kDouble, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::rotationCategory));
	
	// ".w_rotation" attribute
	//
	PRS::w_rotation = fnNumericAttr.create("w_rotation", "wr", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::rotationCategory));

	// ".rotation" attribute
	//
	PRS::rotation = fnNumericAttr.create("rotation", "r", PRS::x_rotation, PRS::y_rotation, PRS::z_rotation, PRS::w_rotation, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::rotationCategory));
	
	// ".axisOrder" attribute
	//
	PRS::axisOrder = fnEnumAttr.create("axisOrder", "ao", short(1), &status);
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
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnEnumAttr.addToCategory(PRS::eulerRotationCategory));

	// ".euler_x_rotation" attribute
	//
	PRS::euler_x_rotation = fnUnitAttr.create("euler_x_rotation", "exr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::eulerRotationCategory));

	// ".euler_y_rotation" attribute
	//
	PRS::euler_y_rotation = fnUnitAttr.create("euler_y_rotation", "eyr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::eulerRotationCategory));

	// ".euler_z_rotation" attribute
	//
	PRS::euler_z_rotation = fnUnitAttr.create("euler_z_rotation", "ezr", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.setKeyable(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(PRS::eulerRotationCategory));

	// ".euler_rotation" attribute
	//
	PRS::euler_rotation = fnNumericAttr.create("euler_rotation", "er", PRS::euler_x_rotation, PRS::euler_y_rotation, PRS::euler_z_rotation, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::eulerRotationCategory));

	// ".x_scale" attribute
	//
	PRS::x_scale = fnNumericAttr.create("x_scale", "xs", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));

	// ".y_scale" attribute
	//
	PRS::y_scale = fnNumericAttr.create("y_scale", "ys",  MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));
	
	// ".z_scale" attribute
	//
	PRS::z_scale = fnNumericAttr.create("z_scale", "zs",  MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	CHECK_MSTATUS(fnNumericAttr.setChannelBox(true));
	CHECK_MSTATUS(fnNumericAttr.setKeyable(true));

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));
	
	// ".scale" attribute
	//
	PRS::scale = fnNumericAttr.create("scale", "s", PRS::x_scale, PRS::y_scale, PRS::z_scale, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::inputCategory));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(PRS::scaleCategory));
	
	// Output attributes:
	// ".matrix" attribute
	//
	PRS::matrix = fnMatrixAttr.create("matrix", "m", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PRS::outputCategory));

	// ".inverseMatrix" attribute
	//
	PRS::inverseMatrix = fnMatrixAttr.create("inverseMatrix", "im", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.addToCategory(PRS::outputCategory));

	// Add attributes to node
	//
	CHECK_MSTATUS(PRS::addAttribute(PRS::position));
	CHECK_MSTATUS(PRS::addAttribute(PRS::rotation));
	CHECK_MSTATUS(PRS::addAttribute(PRS::scale));

	CHECK_MSTATUS(PRS::addAttribute(PRS::matrix));
	CHECK_MSTATUS(PRS::addAttribute(PRS::inverseMatrix));

	// Define position attribute relationships
	//
	CHECK_MSTATUS(PRS::attributeAffects(PRS::position, PRS::matrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::rotation, PRS::matrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::axisOrder, PRS::matrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::euler_rotation, PRS::matrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::scale, PRS::matrix));

	CHECK_MSTATUS(PRS::attributeAffects(PRS::position, PRS::inverseMatrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::rotation, PRS::inverseMatrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::axisOrder, PRS::inverseMatrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::euler_rotation, PRS::inverseMatrix));
	CHECK_MSTATUS(PRS::attributeAffects(PRS::scale, PRS::inverseMatrix));

	return status;

};