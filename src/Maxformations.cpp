//
// File: Maxformations.cpp
//
// Author: Benjamin H. Singleton
//

#include "Maxformations.h"


namespace Maxformations
{

	MMatrix createPositionMatrix(const double x, const double y, const double z)
	/**
	Returns a position matrix from the supplied XYZ values.

	@param x: The X value.
	@param x: The Y value.
	@param x: The Z value.
	@return: The new position matrix.
	*/
	{

		double rows[4][4] = {
			{ 1.0, 0.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0, 0.0 },
			{ 0.0, 0.0, 1.0, 0.0 },
			{ x, y, z, 1.0 },
		};

		return MMatrix(rows);

	};

	MMatrix createPositionMatrix(const MVector& position)
	/**
	Returns a position matrix from the supplied vector.

	@param position: The vector to convert.
	@return: The new position matrix.
	*/
	{

		return createPositionMatrix(position.x, position.y, position.z);

	};

	MMatrix createPositionMatrix(const MMatrix& matrix)
	/**
	Returns a position matrix from the supplied transform matrix.

	@param position: The transform matrix to convert.
	@return: The new position matrix.
	*/
	{

		return createPositionMatrix(matrix(3, 0), matrix(3, 1), matrix(3, 2));

	};

	MMatrix createRotationMatrix(const double x, const double y, const double z, const AxisOrder axisOrder)
	/**
	Creates a rotation matrix from the supplied angles and axis order.

	@param x: The X angle in radians.
	@param y: The Y angle in radians.
	@param z: The Z angle in radians.
	@param axisOrder: The axis order.
	@return: The new rotation matrix.
	*/
	{

		// Compose rotation axis matrices
		//
		double rotateX[4][4] = {
			{1.0, 0.0, 0.0, 0.0},
			{0.0, cos(x), sin(x), 0.0},
			{0.0, -sin(x), cos(x), 0.0},
			{0.0, 0.0, 0.0, 1.0}
		};

		double rotateY[4][4] = {
			{cos(y), 0.0, -sin(y), 0.0},
			{0.0, 1.0, 0.0, 0.0},
			{sin(y), 0.0, cos(y), 0.0},
			{0.0, 0.0, 0.0, 1.0}
		};

		double rotateZ[4][4] = {
			{cos(z), sin(z), 0.0, 0.0},
			{-sin(z), cos(z), 0.0, 0.0},
			{0.0, 0.0, 1.0, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		};

		MMatrix rotateXMatrix = MMatrix(rotateX);
		MMatrix rotateYMatrix = MMatrix(rotateY);
		MMatrix rotateZMatrix = MMatrix(rotateZ);

		// Compose rotation matrix using axis order
		//
		switch (axisOrder)
		{

			case AxisOrder::xyz:
				return rotateXMatrix * rotateYMatrix * rotateZMatrix;

			case AxisOrder::xzy:
				return rotateXMatrix * rotateZMatrix * rotateYMatrix;

			case AxisOrder::yzx:
				return rotateYMatrix * rotateZMatrix * rotateXMatrix;

			case AxisOrder::yxz:
				return rotateYMatrix * rotateXMatrix * rotateZMatrix;

			case AxisOrder::zxy:
				return rotateZMatrix * rotateXMatrix * rotateYMatrix;

			case AxisOrder::zyx:
				return rotateZMatrix * rotateYMatrix * rotateXMatrix;

			case AxisOrder::xyx:
				return rotateXMatrix * rotateYMatrix * rotateXMatrix;

			case AxisOrder::yzy:
				return rotateYMatrix * rotateZMatrix * rotateYMatrix;

			case AxisOrder::zxz:
				return rotateZMatrix * rotateXMatrix * rotateZMatrix;

			default:
				return MMatrix(MMatrix::identity);

		}

	};

	MMatrix createRotationMatrix(const MVector& radians, const AxisOrder axisOrder)
	/**
	Creates a rotation matrix from the supplied radians and axis order.

	@param radians: The XYZ values in radians.
	@param axisOrder: The axis order.
	@return: The new rotation matrix.
	*/
	{

		return createRotationMatrix(radians.x, radians.y, radians.z, axisOrder);

	};

	MMatrix createRotationMatrix(const MVector& radians, const MEulerRotation::RotationOrder rotationOrder)
	/**
	Creates a rotation matrix from the supplied radians and rotation order.

	@param radians: The XYZ values in radians.
	@param axisOrder: The rotation order.
	@return: The new rotation matrix.
	*/
	{

		AxisOrder axisOrder = Maxformations::rotationToAxisOrder(rotationOrder);
		return Maxformations::createRotationMatrix(radians, axisOrder);

	};

	MMatrix createRotationMatrix(const MVector& radians)
	/**
	Creates a rotation matrix from the supplied angles.

	@param radians: The XYZ values in radians.
	@return: The new rotation matrix.
	*/
	{

		return createRotationMatrix(radians.x, radians.y, radians.z, AxisOrder::xyz);

	};

	MMatrix createRotationMatrix(const MMatrix& matrix)
	/**
	Creates a rotation matrix from the supplied transformation matrix.

	@param matrix: The transform matrix to convert.
	@return: The new rotation matrix.
	*/
	{

		MVector xAxis = MVector(matrix[0]).normal();
		MVector yAxis = MVector(matrix[1]).normal();
		MVector zAxis = MVector(matrix[2]).normal();

		double rows[4][4] = {
			{ xAxis.x, xAxis.y, xAxis.z, 0.0 },
			{ yAxis.x, yAxis.y, yAxis.z, 0.0 },
			{ zAxis.x, zAxis.y, zAxis.z, 0.0 },
			{ 0.0, 0.0, 0.0, 1.0 },
		};

		return MMatrix(rows);

	};

	MVector getAxisVector(const int axis, const bool flip)
	/**
	Returns the vector for the specified axis.

	@param axis: The requested axis.
	@param flip: Determines if the axis should be inversed.
	@return: The associated axis vector.
	*/
	{

		switch (axis)
		{

			case 0:
				return flip ? MVector::xNegAxis : MVector::xAxis;

			case 1:
				return flip ? MVector::yNegAxis : MVector::yAxis;

			case 2:
				return flip ? MVector::zNegAxis : MVector::zAxis;

			default:
				return flip ? MVector::xNegAxis : MVector::xAxis;

		}

	};

	MVector getAxisVector(const MMatrix& matrix, const int axis, const bool flip)
	/**
	Returns the vector for the specified axis.

	@param matrix: The matrix to sample from.
	@param axis: The requested axis.
	@param flip: Determines if the axis should be inversed.
	@return: The associated axis vector.
	*/
	{

		switch (axis)
		{

			case 0:
				return flip ? -MVector(matrix[0]) : MVector(matrix[0]);

			case 1:
				return flip ? -MVector(matrix[1]) : MVector(matrix[1]);

			case 2:
				return flip ? -MVector(matrix[2]) : MVector(matrix[2]);

			default:
				return MVector::zero;

		}

	};

	MStatus createAimMatrix(const MVector& forwardVector, const int forwardAxis, const bool forwardAxisFlip, const MVector& upVector, const int upAxis, const bool upAxisFlip, const MPoint& origin, MMatrix& matrix)
	/**
	Creates an aim matrix from the supplied parameters.
	Both of the supplied axes must be unique!

	@param forwardVector: The forward-vector.
	@param forwardAxis: The forward-axis.
	@param forwardAxisFlip: Determines if the forward-axis should be flipped.
	@param upVector: The up-vector.
	@param upAxis: The up-axis.
	@param upAxisFlip: Determines if the up-axis should be flipped.
	@param origin: The point of origin.
	@param matrix: The passed matrix to populate.
	@return: Return status.
	*/
	{

		MStatus status;

		// Evaluate forward-axis
		//
		MVector xAxis = MVector::xAxis;
		MVector yAxis = MVector::yAxis;
		MVector zAxis = MVector::zAxis;

		switch (forwardAxis)
		{

			case 0:
			{

				xAxis = forwardAxisFlip ? MVector(-forwardVector) : MVector(forwardVector);

				if (upAxis == 1)
				{

					zAxis = xAxis ^ (upAxisFlip ? MVector(-upVector) : MVector(upVector));
					yAxis = zAxis ^ xAxis;

				}
				else if (upAxis == 2)
				{

					yAxis = (upAxisFlip ? MVector(-upVector) : MVector(upVector)) ^ xAxis;
					zAxis = xAxis ^ yAxis;

				}
				else
				{

					return MS::kFailure;

				}

			}
			break;

			case 1:
			{

				yAxis = forwardAxisFlip ? MVector(-forwardVector) : MVector(forwardVector);

				if (upAxis == 0)
				{

					zAxis = (upAxisFlip ? MVector(-upVector) : MVector(upVector)) ^ yAxis;
					xAxis = yAxis ^ zAxis;

				}
				else if (upAxis == 2)
				{

					xAxis = yAxis ^ (upAxisFlip ? MVector(-upVector) : MVector(upVector));
					zAxis = xAxis ^ yAxis;

				}
				else
				{

					return MS::kFailure;

				}

			}
			break;

			case 2:
			{

				zAxis = forwardAxisFlip ? MVector(-forwardVector) : MVector(forwardVector);

				if (upAxis == 0)
				{

					yAxis = zAxis ^ (upAxisFlip ? MVector(-upVector) : MVector(upVector));
					xAxis = yAxis ^ zAxis;

				}
				else if (upAxis == 1)
				{

					xAxis = (upAxisFlip ? MVector(-upVector) : MVector(upVector)) ^ zAxis;
					yAxis = zAxis ^ xAxis;

				}
				else
				{

					return MS::kFailure;

				}

			}
			break;

			default:
			{

				return MS::kFailure;

			}

		}

		// Normalize axis vectors
		//
		status = xAxis.normalize();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = yAxis.normalize();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = zAxis.normalize();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Compose aim matrix
		//
		matrix = Maxformations::composeMatrix(xAxis, yAxis, zAxis, origin);
		return MS::kSuccess;

	};

	MStatus createAimMatrix(const MVector& forwardVector, const int forwardAxis, const MVector& upVector, const int upAxis, const MPoint& origin, MMatrix& matrix)
	/**
	Creates an aim matrix from the supplied parameters.
	Both of the supplied axes must be unique!

	@param forwardVector: The forward vector.
	@param forwardAxis: The forward axis.
	@param upVector: The up vector.
	@param upAxis: The up axis.
	@param origin: The point of origin.
	@param matrix: The passed matrix to populate.
	@return: Return status.
	*/
	{

		return Maxformations::createAimMatrix(forwardVector, forwardAxis, false, upVector, upAxis, false, origin, matrix);

	};

	MStatus createAimMatrix(const int forwardAxis, const bool forwardAxisFlip, const int upAxis, const bool upAxisFlip, MMatrix& matrix)
	/**
	Creates an aim matrix from the supplied parameters.
	Both of the supplied axes must be unique!

	@param forwardAxis: The forward axis.
	@param forwardAxisFlip: Determines if the forward axis is inversed.
	@param upAxis: The up axis.
	@param upAxisFlip: Determines if the up axis is inversed.
	@param matrix: The passed matrix to populate.
	@return: Return status.
	*/
	{

		MVector forwardVector = forwardAxisFlip ? MVector::xNegAxis : MVector::xAxis;
		MVector upVector = upAxisFlip ? MVector::yNegAxis : MVector::yAxis;

		return createAimMatrix(forwardVector, forwardAxis, upVector, upAxis, MPoint::origin, matrix);

	};

	MStatus createAimMatrix(const MPointArray& points, const int forwardAxis, const bool forwardAxisFlip, const MVector& upVector, const int upAxis, const bool upAxisFlip, MMatrixArray& matrices)
	/**
	Creates a series of aim matrices through the supplied points.
	Both of the supplied axes must be unique and none of the points should be overlapping!

	@param points: The array of consecutive points.
	@param forwardAxis: The forward axis.
	@param forwardAxisFlip: Determines if the forward axis is inversed.
	@param upVector: The starting up vector.
	@param upAxis: The up axis.
	@param upAxisFlip: Determines if the up axis is inversed.
	@param matrices: The passed matrix array to populate.
	@return: Return status.
	*/
	{

		MStatus status;

		// Resize passed matrix array
		//
		unsigned int numPoints = points.length();

		status = matrices.setLength(numPoints);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Iterate through points
		//
		const unsigned int lastIndex = numPoints - 1;
		double forwardSign = forwardAxisFlip ? -1.0 : 1.0;
		double upSign = upAxisFlip ? -1.0 : 1.0;

		MVector forwardVector, nextUpVector;

		for (unsigned int i = 0; i < numPoints; i++)
		{

			if (i == 0)
			{

				forwardVector = (points[i + 1] - points[i]).normal() * forwardSign;
				nextUpVector = upVector * upSign;

			}
			else if (i == lastIndex)
			{

				forwardVector = (points[i] - points[i - 1]).normal() * forwardSign;
				nextUpVector = MVector(matrices[i - 1][upAxis]);

			}
			else
			{

				forwardVector = (points[i + 1] - points[i]).normal() * forwardSign;
				nextUpVector = MVector(matrices[i - 1][upAxis]);

			}

			status = createAimMatrix(forwardVector, forwardAxis, nextUpVector, upAxis, points[i], matrices[i]);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}
		
		return status;

	};

	MStatus createTwistMatrix(const short forwardAxis, const MAngle& angle, MMatrix& matrix)
	/**
	Creates a twist matrix from the supplied forward axis and angle.

	@param forwardAxis: The forward axis to rotate from.
	@param angle: The angle of rotation.
	@param matrix: The passed matrix to populate.
	@return: Return status.
	*/
	{

		double radian = angle.asRadians();

		switch (forwardAxis)
		{

			case 0:

				matrix = MEulerRotation(radian, 0.0, 0.0).asMatrix();
				break;

			case 1:

				matrix = MEulerRotation(0.0, radian, 0.0).asMatrix();
				break;

			case 2:

				matrix = MEulerRotation(0.0, 0.0, radian).asMatrix();
				break;

			default:

				return MS::kFailure;

		}

		return MS::kSuccess;

	};

	MMatrix createScaleMatrix(const double x, const double y, const double z)
	/**
	Returns a scale matrix from the supplied XYZ values.

	@param x: The X value.
	@param x: The Y value.
	@param x: The Z value.
	@return: The new scale matrix.
	*/
	{

		double rows[4][4] = {
			{ x, 0.0, 0.0, 0.0 },
			{ 0.0, y, 0.0, 0.0 },
			{ 0.0, 0.0, z, 0.0 },
			{ 0.0, 0.0, 0.0, 1.0 },
		};

		return MMatrix(rows);

	};

	MMatrix createScaleMatrix(const MVector& scale)
	/**
	Returns a scale matrix from the supplied scale vector.

	@param scale: The vector to convert.
	@return: The new scale matrix.
	*/
	{

		return createScaleMatrix(scale.x, scale.y, scale.z);

	};

	MMatrix createScaleMatrix(const MMatrix& matrix)
	/**
	Returns a scale matrix from the supplied transformation matrix.

	@param matrix: The transform matrix to convert.
	@return: The new scale matrix.
	*/
	{

		return createScaleMatrix(matrixToScale(matrix));

	};

	MMatrix composeMatrix(const MVector& xAxis, const MVector& yAxis, const MVector& zAxis, const MPoint& position)
	/**
	Returns a matrix from the supplied axis vectors and position.

	@param xAxis: The x-axis vector.
	@param yAxis: The y-axis vector.
	@param zAxis: The z-axis vector.
	@param position: The positional value.
	@return: Transform matrix.
	*/
	{

		double rows[4][4] = {
			{xAxis.x, xAxis.y, xAxis.z, 0.0},
			{yAxis.x, yAxis.y, yAxis.z, 0.0},
			{zAxis.x, zAxis.y, zAxis.z, 0.0},
			{position.x, position.y, position.z, 1.0}
		};

		return MMatrix(rows);

	};
	
	MStatus composeMatrix(const MObject& mesh, const unsigned int triangleIndex, const MVector& baryCoords, MMatrix& matrix)
	/**
	Composes a transformation matrix from the supplied mesh.

	@param mesh: The passed mesh to sample from.
	@param triangleIndex: The triangle index to sample from.
	@param baryCoords: The barycentric co-ordinates for the specified triangle.
	@param matrix: The passed transformation matrix to populate.
	@return: Status code.
	*/
	{

		MStatus status;

		// Initialize mesh function set
		//
		MFnMesh fnMesh(mesh, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Evaluate mesh triangulation
		//
		MIntArray triangleCounts, triangleVertices;

		status = fnMesh.getTriangles(triangleCounts, triangleVertices);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		unsigned int numTriangles = Maxformations::sum(triangleCounts);

		// Get triangle-vertex indices
		//
		unsigned int normalizedTriangleIndex = Maxformations::loop(triangleIndex, (unsigned int)0, numTriangles);

		unsigned int triangleOffset = normalizedTriangleIndex * 3;
		unsigned int v1 = triangleVertices[triangleOffset];
		unsigned int v2 = triangleVertices[triangleOffset + 1];
		unsigned int v3 = triangleVertices[triangleOffset + 2];

		// Get triangle-vertex points
		//
		MPoint p1, p2, p3;

		status = fnMesh.getPoint(v1, p1, MSpace::kWorld);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = fnMesh.getPoint(v2, p2, MSpace::kWorld);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = fnMesh.getPoint(v3, p3, MSpace::kWorld);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Calculate position from barycentric average
		//
		MPoint position = (p1 * baryCoords.x) + (p2 * baryCoords.y) + (p3 * baryCoords.z);

		// Calculate transform matrix axes
		//
		MVector xAxis = (p2 - p1).normal();
		MVector zAxis = ((p2 - p1).normal() ^ (p3 - p1).normal()).normal();
		MVector yAxis = (zAxis ^ xAxis).normal();

		matrix = Maxformations::composeMatrix(xAxis, yAxis, zAxis, position);

		return MS::kSuccess;

	};

	void decomposeMatrix(const MMatrix& matrix, MPoint& position, MQuaternion& rotation, MVector& scale)
	/**
	Decomposes the supplied transformation matrix into its separate position, rotation and scale components.

	@param matrix: The transformation matrix to decompose.
	@param position: Passed point to populate.
	@param rotation: Passed quaternion to populate.
	@param scale: Passed vector to populate.
	@return: Void.
	*/
	{

		position = Maxformations::matrixToPosition(matrix);
		rotation = Maxformations::matrixToQuaternion(matrix);
		scale = Maxformations::matrixToScale(matrix);

	};

	void breakMatrix(const MMatrix& matrix, MVector& xAxis, MVector& yAxis, MVector& zAxis, MPoint& position)
		/**
		Breaks the supplied matrix into its axis vector and position components.

		@param matrix: The matrix to break.
		@param xAxis: The x-axis vector.
		@param yAxis: The y-axis vector.
		@param zAxis: The z-axis vector.
		@param position: The positional value.
		@return: Void.
		*/
	{

		xAxis = MVector(matrix[0]);
		yAxis = MVector(matrix[1]);
		zAxis = MVector(matrix[2]);
		position = MVector(matrix[3]);

	};

	MMatrix normalizeMatrix(const MMatrix& matrix)
	/**
	Returns a normalized matrix from the supplied transform matrix.

	@param matrix: The matrix to normalize.
	@return: Normalized matrix.
	*/
	{

		// Break matrix
		//
		MVector xAxis, yAxis, zAxis;
		MPoint position;

		breakMatrix(matrix, xAxis, yAxis, zAxis, position);

		// Normalize axis vectors
		//
		xAxis.normalize();
		yAxis.normalize();
		zAxis.normalize();

		// Compose new matrix
		//
		return Maxformations::composeMatrix(xAxis, yAxis, zAxis, position);

	};

	MMatrixArray staggerMatrices(const MMatrixArray& matrices)
	/**
	Returns staggered matrices where each matrix is converted to local space using the preceding matrix as its parent space.

	@param matrices: The matrices to stagger.
	@return: The matrices in local space.
	*/
	{
		
		MMatrixArray newMatrices = MMatrixArray(matrices);
		unsigned int numMatrices = newMatrices.length();

		for (unsigned int i = 1; i < numMatrices; i++)  // Skip the first item!
		{

			newMatrices[i] = matrices[i] * matrices[i - 1].inverse();

		}

		return newMatrices;

	};

	MMatrixArray expandMatrices(const MMatrixArray& matrices)
	/**
	Returns expanded matrices where each matrix is converted to world space using the preceding matrix as its parent space.

	@param matrices: The matrices to stagger.
	@return: The matrices in world space.
	*/
	{

		MMatrixArray newMatrices = MMatrixArray(matrices);
		unsigned int numMatrices = newMatrices.length();

		for (unsigned int i = 1; i < numMatrices; i++)  // Skip the first item!
		{

			newMatrices[i] = matrices[i] * newMatrices[i - 1];

		}

		return newMatrices;

	};

	MStatus twistMatrices(MMatrixArray& matrices, const int forwardAxis, const MAngle& startTwistAngle, const MAngle& endTwistAngle)
	/**
	Twists the passed matrices based on the specified start and end twist angles.

	@param matrices: The matrices to twist.
	@param forwardAxis: The forward axis.
	@param startTwistAngle: The start twist angle.
	@param endTwistAngle: The end twist angle.
	@return: Return status.
	*/
	{

		// Check if there are enough matrices
		// Otherwise we'll encounter divide by zero errors!
		//
		unsigned int numMatrices = matrices.length();

		if (numMatrices == 0 || !(0 <= forwardAxis && forwardAxis < 3))
		{

			return MS::kFailure;

		}

		// Iterate through matrices
		//
		double fraction = 1.0 / static_cast<double>(numMatrices - 1);

		double weight, radian, accumulated = 0.0;
		MEulerRotation eulerRotation;

		for (unsigned int i = 0; i < numMatrices; i++)
		{

			weight = fraction * static_cast<double>(i);
			radian = lerp(startTwistAngle.asRadians(), endTwistAngle.asRadians(), weight);

			eulerRotation = MEulerRotation(0.0, 0.0, 0.0);
			eulerRotation[forwardAxis] = radian + accumulated;

			matrices[i] = eulerRotation.asMatrix() * matrices[i];
			accumulated += radian;

		}

		return MS::kSuccess;

	};

	MStatus reorientMatrices(MMatrixArray& matrices, int forwardAxis, bool forwardAxisFlip, int upAxis, bool upAxisFlip)
	/**
	Re-orients the passed matrices based on the supplied axis alignments.

	@param matrices: The matrices to re-orient.
	@param forwardAxis: The forward-axis.
	@param forwardAxisFlip: Determines if the forward-axis should be flipped.
	@param upAxis: The up-axis.
	@param upAxisFlip: Determines if the up-axis should be flipped.
	@return: The re-oriented matrices.
	*/
	{

		MStatus status;

		// Compose aim matrix
		//
		MMatrix aimMatrix;

		status = createAimMatrix(forwardAxis, forwardAxisFlip, upAxis, upAxisFlip, aimMatrix);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Re-orient matrices
		//
		unsigned int numMatrices = matrices.length();

		for (unsigned int i = 0; i < numMatrices; i++)
		{

			matrices[i] = aimMatrix * matrices[i];

		}

		return MS::kSuccess;

	};

	MVector matrixToPosition(const MMatrix& matrix)
	/**
	Extracts the position component from the supplied transform matrix.

	@param matrix: The transform matrix to extract from.
	@return: The position value.
	*/
	{

		return MVector(matrix[3]);

	};

	MDistance distanceBetween(const MMatrix& startMatrix, const MMatrix& endMatrix)
	/**
	Evaluates the distance between the two matrices.

	@param startMatrix: The start matrix.
	@param endMatrix: The end matrix.
	@return: The distance in UI units.
	*/
	{

		MPoint startPoint = matrixToPosition(startMatrix);
		MPoint endPoint = matrixToPosition(endMatrix);

		return MDistance(startPoint.distanceTo(endPoint), MDistance::uiUnit());

	};

	MEulerRotation::RotationOrder axisToRotationOrder(const AxisOrder axisOrder)
	/**
	Converts the supplied axis order to a rotation order.

	@param axisOrder: The axis order to convert.
	@return: The equivalent rotation order.
	*/
	{

		switch (axisOrder)
		{

		case AxisOrder::xyz:
			return MEulerRotation::kXYZ;

		case AxisOrder::xzy:
			return MEulerRotation::kXZY;

		case AxisOrder::yzx:
			return MEulerRotation::kYZX;

		case AxisOrder::yxz:
			return MEulerRotation::kYXZ;

		case AxisOrder::zxy:
			return MEulerRotation::kZXY;

		case AxisOrder::zyx:
			return MEulerRotation::kZYX;

		default:
			return MEulerRotation::kXYZ;

		}

	};

	AxisOrder rotationToAxisOrder(const MEulerRotation::RotationOrder rotationOrder)
	/**
	Converts the supplied rotation order to an axis order.

	@param axisOrder: The rotation order to convert.
	@return: The equivalent axis order.
	*/
	{

		switch (rotationOrder)
		{

		case MEulerRotation::kXYZ:
			return AxisOrder::xyz;

		case MEulerRotation::kXZY:
			return AxisOrder::xzy;

		case MEulerRotation::kYZX:
			return AxisOrder::yzx;

		case MEulerRotation::kYXZ:
			return AxisOrder::yxz;

		case MEulerRotation::kZXY:
			return AxisOrder::zxy;

		case MEulerRotation::kZYX:
			return AxisOrder::zyx;

		default:
			return AxisOrder::xyz;

		}

	};

	MVector matrixToEulerXYZ(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `xyz` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double x, y, z;

		if (matrix(0, 2) < 1.0)
		{

			if (matrix(0, 2) > -1.0)
			{

				y = asin(matrix(0, 2));
				x = atan2(-matrix(1, 2), matrix(2, 2));
				z = atan2(-matrix(0, 1), matrix(0, 0));

			}
			else
			{

				y = -M_PI / 2.0;
				x = -atan2(matrix(1, 0), matrix(1, 1));
				z = 0.0;

			}

		}
		else
		{

			y = M_PI / 2.0;
			x = atan2(matrix(1, 0), matrix(1, 1));
			z = 0.0;

		}

		return MVector(-x, -y, -z);

	};

	MVector matrixToEulerXZY(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `xzy` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double x, z, y;

		if (matrix(0, 1) < 1.0)
		{

			if (matrix(0, 1) > -1.0)
			{

				z = asin(-matrix(0, 1));
				x = atan2(matrix(2, 1), matrix(1, 1));
				y = atan2(matrix(0, 2), matrix(0, 0));

			}
			else
			{

				z = M_PI / 2.0;
				x = -atan2(-matrix(2, 0), matrix(2, 2));
				y = 0.0;

			}

		}
		else
		{

			z = -M_PI / 2.0;
			x = atan2(-matrix(2, 0), matrix(2, 2));
			y = 0.0;

		}

		return MVector(-x, -z, -y);

	};

	MVector matrixToEulerYXZ(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `yxz` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double y, x, z;

		if (matrix(1, 2) < 1.0)
		{

			if (matrix(1, 2) > -1.0)
			{

				x = asin(-matrix(1, 2));
				y = atan2(matrix(0, 2), matrix(2, 2));
				z = atan2(matrix(1, 0), matrix(1, 1));

			}
			else
			{

				x = M_PI / 2.0;
				y = -atan2(-matrix(0, 1), matrix(0, 0));
				z = 0.0;

			}

		}
		else
		{

			x = -M_PI / 2.0;
			y = atan2(-matrix(0, 1), matrix(0, 0));
			z = 0.0;

		}

		return MVector(-y, -x, -z);;

	};

	MVector matrixToEulerYZX(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `yzx` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double y, x, z;

		if (matrix(1, 0) < 1.0)
		{

			if (matrix(1, 0) > -1.0)
			{

				z = asin(matrix(1, 0));
				y = atan2(-matrix(2, 0), matrix(0, 0));
				x = atan2(-matrix(1, 2), matrix(1, 1));

			}
			else
			{

				z = -M_PI / 2.0;
				y = -atan2(matrix(2, 1), matrix(2, 2));
				x = 0.0;

			}

		}
		else
		{

			z = M_PI / 2.0;
			y = atan2(matrix(2, 1), matrix(2, 2));
			x = 0.0;

		}

		return MVector(-y, -z, -x);

	};

	MVector matrixToEulerZXY(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `zxy` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double z, x, y;

		if (matrix(2, 1) < 1.0)
		{

			if (matrix(2, 1) > -1.0)
			{

				x = asin(matrix(2, 1));
				z = atan2(-matrix(0, 1), matrix(1, 1));
				y = atan2(-matrix(2, 0), matrix(2, 2));

			}
			else
			{

				x = -M_PI / 2.0;
				z = -atan2(matrix(0, 2), matrix(0, 0));
				y = 0.0;

			}

		}
		else
		{

			x = M_PI / 2.0;
			z = atan2(matrix(0, 2), matrix(0, 0));
			y = 0.0;

		}

		return MVector(-z, -x, -y);

	};

	MVector matrixToEulerZYX(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `zyx` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double z, y, x;

		if (matrix(2, 0) < 1.0)
		{

			if (matrix(2, 0) > -1.0)
			{

				y = asin(-matrix(2, 0));
				z = atan2(matrix(1, 0), matrix(0, 0));
				x = atan2(matrix(2, 1), matrix(2, 2));

			}
			else
			{

				y = M_PI / 2.0;
				z = -atan2(-matrix(1, 2), matrix(1, 1));
				x = 0.0;

			}

		}
		else
		{

			y = -M_PI / 2.0;
			z = atan2(-matrix(1, 2), matrix(1, 1));
			x = 0.0;

		}

		return MVector(-z, -y, -x);

	};

	MVector matrixToEulerXYX(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `xyx` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double x0, y, x1;

		if (matrix(0, 0) < 1.0)
		{

			if (matrix(0, 0) > -1.0)
			{

				y = acos(matrix(0, 0));
				x0 = atan2(matrix(1, 0), -matrix(2, 0));
				x1 = atan2(matrix(0, 1), matrix(0, 2));

			}
			else
			{

				y = M_PI;
				x0 = -atan2(-matrix(1, 2), matrix(1, 1));
				x1 = 0.0;

			}

		}
		else
		{

			y = 0.0;
			x0 = atan2(-matrix(1, 2), matrix(1, 1));
			x1 = 0.0;

		}

		return MVector(x0, y, x1);

	};

	MVector matrixToEulerYZY(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `yzy` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double y0, z, y1;

		if (matrix(1, 1) < 1.0)
		{

			if (matrix(1, 1) > -1.0)
			{

				z = acos(matrix(1, 1));
				y0 = atan2(matrix(2, 1), -matrix(0, 1));
				y1 = atan2(matrix(1, 2), matrix(1, 0));

			}
			else
			{

				z = M_PI;
				y0 = -atan2(-matrix(2, 0), matrix(2, 2));
				y1 = 0.0;

			}

		}
		else
		{

			z = 0.0;
			y0 = atan2(-matrix(2, 0), matrix(2, 2));
			y1 = 0.0;

		}

		return MVector(y0, z, y1);

	};

	MVector matrixToEulerZXZ(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into euler angles using the `zxz` axis order.

	@param matrix: The matrix to extract from.
	@return: The euler angles in radians.
	*/
	{

		double z0, x, z1;

		if (matrix(2, 2) < 1.0)
		{

			if (matrix(2, 2) > -1.0)
			{

				x = acos(matrix(2, 2));
				z0 = atan2(matrix(0, 2), -matrix(1, 2));
				z1 = atan2(matrix(2, 0), matrix(2, 1));

			}
			else
			{

				x = M_PI;
				z0 = -atan2(-matrix(0, 1), matrix(0, 0));
				z1 = 0.0;

			}

		}
		else
		{

			x = 0.0;
			z0 = atan2(-matrix(0, 1), matrix(0, 0));
			z1 = 0.0;

		}

		return MVector(z0, x, z1);

	};

	MVector matrixToEulerAngles(const MMatrix& matrix, const AxisOrder axisOrder)
	/**
	Converts the supplied transform matrix into euler angles using the specified axis order.

	@param matrix: The matrix to convert.
	@param axis: The axis order for the euler angles.
	@return: The angles in radians.
	*/
	{

		switch (axisOrder)
		{

		case AxisOrder::xyz:
			return matrixToEulerXYZ(matrix);

		case AxisOrder::xzy:
			return matrixToEulerXZY(matrix);

		case AxisOrder::yzx:
			return matrixToEulerYZX(matrix);

		case AxisOrder::yxz:
			return matrixToEulerYXZ(matrix);

		case AxisOrder::zxy:
			return matrixToEulerZXY(matrix);

		case AxisOrder::zyx:
			return matrixToEulerZYX(matrix);

		case AxisOrder::xyx:
			return matrixToEulerXYX(matrix);

		case AxisOrder::yzy:
			return matrixToEulerYZY(matrix);

		case AxisOrder::zxz:
			return matrixToEulerZXZ(matrix);

		default:
			return matrixToEulerXYZ(matrix);

		}

	};

	MEulerRotation matrixToEulerRotation(const MMatrix& matrix, const AxisOrder axisOrder)
	/**
	Converts the supplied transform matrix into an euler rotation using the specified axis order.

	@param matrix: The matrix to convert.
	@param axis: The axis order for the euler angles.
	@return: The euler rotation.
	*/
	{

		MEulerRotation::RotationOrder rotationOrder = axisToRotationOrder(axisOrder);
		return matrixToEulerRotation(matrix, rotationOrder);

	};

	MEulerRotation matrixToEulerRotation(const MMatrix& matrix, const MEulerRotation::RotationOrder rotationOrder)
	/**
	Converts the supplied transform matrix into an euler rotation using the specified rotation order.

	@param matrix: The matrix to convert.
	@param axis: The rotation order for the euler angles.
	@return: The euler rotation.
	*/
	{

		MEulerRotation eulerRotation;
		eulerRotation = matrix;
		eulerRotation.reorderIt(rotationOrder);

		return eulerRotation;

	};

	MEulerRotation matrixToEulerRotation(const MMatrix& matrix, const MTransformationMatrix::RotationOrder rotationOrder)
	/**
	Converts the supplied transform matrix into an euler rotation using the specified rotation order.

	@param matrix: The matrix to convert.
	@param axis: The rotation order for the euler angles.
	@return: The euler rotation.
	*/
	{

		return matrixToEulerRotation(matrix, MEulerRotation::RotationOrder(rotationOrder - 1));

	};

	MQuaternion eulerAnglesToQuaternion(const MVector& radians, const AxisOrder axisOrder)
	/**
	Converts the supplied euler angles into a quaternion.

	@param radians: The XYZ values in radians.
	@param axisOrder: The axis order.
	@return: The transform orientation as a quaternion.
	*/
	{

		return matrixToQuaternion(createRotationMatrix(radians, axisOrder));

	};

	MQuaternion matrixToQuaternion(const MMatrix& matrix)
	/**
	Converts the supplied transform matrix into a quaternion.

	@param matrix: The matrix to convert.
	@return: The transform orientation as a quaternion.
	*/
	{

		MQuaternion quat;
		quat = createRotationMatrix(matrix);

		return quat;

	};
	
	double dot(const MQuaternion& quat, const MQuaternion& otherQuat)
	/**
	Returns the dot product of two quaternions.

	@param quat: Quaternion.
	@param: otherQuat: Other quaternion.
	@return: Dot length.
	*/
	{

		return (quat.x * otherQuat.x) + (quat.y * otherQuat.y) + (quat.z * otherQuat.z) + (quat.w * otherQuat.w);

	};

	MQuaternion slerp(const MQuaternion& startQuat, const MQuaternion& endQuat, const float weight)
	/**
	Spherical interpolates two quaternions.
	See the following for details: https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm

	@param startQuat: Start Quaternion.
	@param endQuat: End Quaternion.
	@param weight: The amount to interpolate.
	@return: The interpolated quaternion.
	*/
	{

		MQuaternion q1 = MQuaternion(startQuat);
		MQuaternion q2 = MQuaternion(endQuat);

		double dot = Maxformations::dot(q1, q2);

		if (dot < 0.0)
		{

			dot = Maxformations::dot(q1, q2.negateIt());

		}

		double theta = acos(dot);
		double sinTheta = sin(theta);

		double w1, w2;

		if (sinTheta > 1e-3)
		{

			w1 = sin((1.0 - weight) * theta) / sinTheta;
			w2 = sin(weight * theta) / sinTheta;

		}
		else
		{

			w1 = 1.0 - weight;
			w2 = weight;

		}

		q1.scaleIt(w1);
		q2.scaleIt(w2);

		return q1 + q2;

	};

	MMatrix slerp(const MMatrix& startMatrix, const MMatrix& endMatrix, const float weight)
	/**
	Spherical interpolates two rotation matrices.

	@param startQuat: Start matrix.
	@param endQuat: End matrix.
	@param weight: The amount to interpolate.
	@return: The interpolated matrix.
	*/
	{

		MQuaternion startQuat;
		startQuat = startMatrix;

		MQuaternion endQuat;
		endQuat = endMatrix;

		return Maxformations::slerp(startQuat, endQuat, weight).asMatrix();

	};

	MAngle angleBetween(const MMatrix& startMatrix, const MMatrix& endMatrix)
	/**
	Evaluates the angle between the two matrices.

	@param startMatrix: The start matrix.
	@param endMatrix: The end matrix.
	@return: The angle in radians.
	*/
	{

		MVector startVector = matrixToPosition(startMatrix).normal();
		MVector endVector = matrixToPosition(endMatrix).normal();

		return MAngle(startVector.angle(endVector), MAngle::Unit::kRadians);

	};

	MVector matrixToScale(const MMatrix& matrix)
	/**
	Extracts the scale component from the supplied transform matrix.

	@param matrix: The transform matrix to extract from.
	@return: The scale value.
	*/
	{

		MVector xAxis, yAxis, zAxis;
		MPoint position;

		breakMatrix(matrix, xAxis, yAxis, zAxis, position);

		return MVector(xAxis.length(), yAxis.length(), zAxis.length());

	};

	float sum(const MFloatArray& items)
	/**
	Calculates the sum of all the supplied items.

	@param items: The items to add up.
	@return: The total sum.
	*/
	{

		// Iterate through numbers
		//
		unsigned int numItems = items.length();
		float sum = 0.0;

		for (unsigned int i = 0; i < numItems; i++)
		{

			sum += items[i];

		}

		return sum;

	};

	unsigned int sum(const MIntArray& items)
	/**
	Calculates the sum of all the supplied items.

	@param items: The items to add up.
	@return: The total sum.
	*/
	{

		// Iterate through numbers
		//
		unsigned int numItems = items.length();
		unsigned int sum = 0;

		for (unsigned int i = 0; i < numItems; i++)
		{

			sum += items[i];

		}

		return sum;

	};

	MFloatArray clamp(const MFloatArray& items)
	/**
	Clamps the supplied items so they don't exceed 100.
	Anything below that is left alone and compensated for using the rest matrix.

	@param items: The float array containing the weighted averages.
	@return: The newly clamped array of weights.
	*/
	{

		// Check if weights require clamping
		//
		float sum = Maxformations::sum(items);

		if (sum < 1.0f)
		{

			return MFloatArray(items);

		}

		// Normalize weights
		//
		float fraction = 1.0f / sum;

		unsigned int numItems = items.length();
		MFloatArray normalizedItems = MFloatArray(numItems);

		for (unsigned int i = 0; i < numItems; i++)
		{

			normalizedItems[i] = items[i] * fraction;

		}

		return normalizedItems;

	};

	MMatrix blendMatrices(const MMatrix& startMatrix, const MMatrix& endMatrix, const float weight)
	/**
	Interpolates the two given matrices using the supplied weight.
	Both translate and scale will be lerp'd while rotation will be slerp'd.

	@param startMatrix: The start matrix.
	@param endMatrix: The end matrix.
	@param weight: The amount to blend.
	@return: The interpolated matrix.
	*/
	{

		MStatus status;

		// Check if matrices are equivalent
		//
		bool isEquivalent = startMatrix.isEquivalent(endMatrix);

		if (isEquivalent)
		{

			return startMatrix;

		}

		// Check matrices require blending
		//
		if (0.0f < weight && weight < 1.0f)
		{

			// Decompose transform matrices
			//
			MPoint startPosition, endPosition;
			MVector startScale, endScale;
			MQuaternion startRotation, endRotation;

			Maxformations::decomposeMatrix(startMatrix, startPosition, startRotation, startScale);
			Maxformations::decomposeMatrix(endMatrix, endPosition, endRotation, endScale);

			// Interpolate translation
			//
			MPoint translation = Maxformations::lerp(startPosition, endPosition, weight);
			MQuaternion quat = Maxformations::slerp(startRotation, endRotation, weight);
			MVector scale = Maxformations::lerp(startScale, endScale, weight);

			// Compose interpolated matrix
			//
			MMatrix translateMatrix = Maxformations::createPositionMatrix(translation);
			MMatrix rotateMatrix = quat.asMatrix();
			MMatrix scaleMatrix = Maxformations::createScaleMatrix(scale);

			return scaleMatrix * rotateMatrix * translateMatrix;

		}
		else
		{

			return (weight == 0.0f) ? startMatrix : endMatrix;

		}

	};

	MMatrix blendMatrices(const MMatrix& restMatrix, const MMatrixArray& matrices, const MFloatArray& weights)
	/**
	Interpolates the supplied matrices using the weight array as a blend aplha.
	The rest matrix is used just in case the weights don't equal 100.

	@param restMatrix: The default matrix to blend from in case the weights don't equal 100.
	@param matrices: The matrix array to blend.
	@param weights: The float array containing the weighted averages.
	@return: The interpolated matrix.
	*/
	{

		// Check the number of matrices
		//
		unsigned int numMatrices = matrices.length();

		switch (numMatrices)
		{

			case 0:
			{

				return MMatrix(restMatrix);

			}
			break;

			case 1:
			{

				// Evaluate weight sum
				//
				float weightSum = Maxformations::sum(weights);

				if (0.0f < weightSum && weightSum <= 1.0f)
				{

					return MMatrix(matrices[0]);

				}
				else
				{

					return MMatrix(restMatrix);

				}

			}
			break;

			default:
			{

				// Evaluate weight sum
				//
				MFloatArray normalizedWeights = Maxformations::clamp(weights);
				float weightSum = Maxformations::sum(normalizedWeights);

				if (0.0f < weightSum && weightSum <= 1.0f)
				{

					// Lerp through matrices
					//
					MMatrix matrix = MMatrix(matrices[0]);
					unsigned int numMatrices = matrices.length();

					for (unsigned int i = 1; i < numMatrices; i++)
					{

						matrix = Maxformations::blendMatrices(matrix, matrices[i], normalizedWeights[i]);

					}

					return matrix;

				}
				else
				{

					return MMatrix(restMatrix);

				}
				

			}
			break;

		}

		return MMatrix::identity;

	};

	MObject createMatrixData(const MMatrix& matrix)
	/**
	Returns a matrix data object from the supplied matrix.

	@param matrix: The matrix to convert.
	@param status: Status code.
	@return: Matrix data object.
	*/
	{

		MStatus status;

		// Create new matrix data
		//
		MFnMatrixData fnMatrixData;

		MObject matrixData = fnMatrixData.create(&status);
		CHECK_MSTATUS_AND_RETURN(status, MObject::kNullObj);

		// Assign identity matrix
		//
		status = fnMatrixData.set(matrix);
		CHECK_MSTATUS_AND_RETURN(status, MObject::kNullObj);

		return matrixData;

	};

	MObject createMatrixData(const MTransformationMatrix& transform)
	/**
	Returns a matrix data object from the supplied transformation matrix.

	@param matrix: The transformation matrix to convert.
	@param status: Status code.
	@return: Matrix data object.
	*/
	{

		MStatus status;

		// Create new matrix data
		//
		MFnMatrixData fnMatrixData;

		MObject matrixData = fnMatrixData.create(&status);
		CHECK_MSTATUS_AND_RETURN(status, MObject::kNullObj);

		// Assign identity matrix
		//
		status = fnMatrixData.set(transform);
		CHECK_MSTATUS_AND_RETURN(status, MObject::kNullObj);

		return matrixData;

	};

	MMatrix	getMatrixData(const MObject& matrixData)
	/**
	Returns the matrix value from the supplied matrix data object.

	@param matrixData: The matrix data object.
	@param status: Status code.
	@return: The matrix value.
	*/
	{

		MStatus status;

		MFnMatrixData fnMatrixData(matrixData, &status);
		CHECK_MSTATUS_AND_RETURN(status, MMatrix::identity);

		MMatrix matrix = fnMatrixData.matrix(&status);
		CHECK_MSTATUS_AND_RETURN(status, MMatrix::identity);

		return matrix;

	};

	MTransformationMatrix getTransformData(const MObject& matrixData)
	/**
	Returns the transformation matrix from the supplied matrix data object.

	@param matrixData: The matrix data object.
	@param status: Status code.
	@return: The matrix value.
	*/
	{

		MStatus status;

		MFnMatrixData fnMatrixData(matrixData, &status);
		CHECK_MSTATUS_AND_RETURN(status, MTransformationMatrix::identity);

		MTransformationMatrix transformationMatrix = fnMatrixData.transformation(&status);
		CHECK_MSTATUS_AND_RETURN(status, MTransformationMatrix::identity);

		return transformationMatrix;

	};

	MStatus resetMatrixPlug(MPlug& plug)
	/**
	Resets the matrix value on the supplied plug.

	@param plug: The plug to reset.
	@return: Status code.
	*/
	{

		MStatus status;

		// Assign identity matrix to plug
		//
		MObject matrixData = createMatrixData(MMatrix::identity);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = plug.setMObject(matrixData);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return status;

	};

	bool isPartiallyConnected(const MPlug& plug, const bool asDst, const bool asSrc, MStatus* status)
	/**
	Evaluates if the supplied plug is partially connected to another node.
	This includes child plugs and plug elements.

	@param plug: The plug to inspect.
	@param asDst: Whether incoming connections should be tested.
	@param asSrc: Whether outgoing connections should be tested.
	@param status: Status code.
	@return: Is connected.
	*/
	{

		// Check for null plug
		//
		bool isNull = plug.isNull(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		if (isNull)
		{

			return false;

		}

		// Check if this is a compound plug
		//
		bool isArray = plug.isArray(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		bool isElement = plug.isElement(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		bool isCompound = plug.isCompound(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		if (isArray && !isElement)
		{

			// Iterate through children
			//
			unsigned int numElements = plug.numElements(status);
			CHECK_MSTATUS_AND_RETURN(*status, false);

			MPlug element;
			bool isConnected;

			for (unsigned int i = 0; i < numElements; i++)
			{

				// Evaluate if plug element is partially connected
				//
				element = plug.elementByPhysicalIndex(i, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				isConnected = isPartiallyConnected(element, asDst, asSrc, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				if (isConnected)
				{

					return true;

				}

			}

			return false;

		}
		else if (isCompound)
		{

			// Iterate through children
			//
			unsigned int numChildren = plug.numChildren(status);
			CHECK_MSTATUS_AND_RETURN(*status, false);

			MPlug child;
			bool isConnected;

			for (unsigned int i = 0; i < numChildren; i++)
			{

				// Evalaute if child plug is partially connected
				//
				child = plug.child(i, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				isConnected = isPartiallyConnected(child, asDst, asSrc, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				if (isConnected)
				{

					return true;

				}

			}

			return false;

		}
		else
		{

			// Evaluate incoming connection
			//
			MPlugArray connections;

			bool isConnected = plug.connectedTo(connections, asDst, asSrc, status);
			CHECK_MSTATUS_AND_RETURN(*status, false);

			unsigned int connectionCount = connections.length();

			return (isConnected && connectionCount > 0u);

		}

	};

	bool isPartiallyLocked(const MPlug& plug, MStatus* status)
	/**
	Evaluates if the supplied plug is partially locked.
	This includes child plugs and plug elements.

	@param plug: The plug to inspect.
	@param status: Status code.
	@return: Is locked.
	*/
	{

		// Check for null plug
		//
		bool isNull = plug.isNull(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		if (isNull)
		{

			return false;

		}

		// Check if this is a compound plug
		//
		bool isArray = plug.isArray(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		bool isElement = plug.isElement(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		bool isCompound = plug.isCompound(status);
		CHECK_MSTATUS_AND_RETURN(*status, false);

		if (isArray && !isElement)
		{

			// Iterate through children
			//
			unsigned int numElements = plug.numElements(status);
			CHECK_MSTATUS_AND_RETURN(*status, false);

			MPlug element;
			bool isLocked;

			for (unsigned int i = 0; i < numElements; i++)
			{

				// Evaluate if plug element is partially connected
				//
				element = plug.elementByPhysicalIndex(i, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				isLocked = isPartiallyLocked(element, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				if (isLocked)
				{

					return true;

				}

			}

			return false;

		}
		else if (isCompound)
		{

			// Iterate through children
			//
			unsigned int numChildren = plug.numChildren(status);
			CHECK_MSTATUS_AND_RETURN(*status, false);

			MPlug child;
			bool isLocked;

			for (unsigned int i = 0; i < numChildren; i++)
			{

				// Evalaute if child plug is partially connected
				//
				child = plug.child(i, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				isLocked = isPartiallyLocked(child, status);
				CHECK_MSTATUS_AND_RETURN(*status, false);

				if (isLocked)
				{

					return true;

				}

			}

			return false;

		}
		else
		{

			// Evaluate plug
			//
			bool isLocked = plug.isLocked(status);
			CHECK_MSTATUS_AND_RETURN(*status, false);

			return isLocked;

		}

	};

	MStatus disconnectPlugs(const MPlug& plug, const MPlug& otherPlug)
	/**
	Disconnects the two supplied plugs.

	@param plug: The source plug.
	@param otherPlug: The destination plug.
	@return: Status code.
	*/
	{

		MStatus status;

		// Check for null plugs
		//
		bool isNull = plug.isNull(&status) || otherPlug.isNull(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isNull)
		{

			return MS::kFailure;

		}

		// Setup dag modifier
		//
		MDGModifier modifier;

		status = modifier.disconnect(plug, otherPlug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Execute modifier
		//
		status = modifier.doIt();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return status;

	};

	MStatus breakConnections(const MPlug& plug, bool asDst, bool asSrc)
	/**
	Breaks the connections to the supplied plug.

	@param plug: The plug to break connections on.
	@param source: Break input connections.
	@param destination: Break output connections.
	@return: Status code.
	*/
	{

		MStatus status;

		// Check for null plug
		//
		bool isNull = plug.isNull(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isNull)
		{

			return MS::kFailure;

		}

		// Check if this is a compound plug
		//
		bool isArray = plug.isArray(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isElement = plug.isElement(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		bool isCompound = plug.isCompound(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isArray && !isElement)
		{

			// Iterate through elements
			//
			unsigned int numElements = plug.numElements(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug element;

			for (unsigned int i = 0; i < numElements; i++)
			{

				element = plug.elementByPhysicalIndex(i, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				status = breakConnections(element, asDst, asSrc);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

		}
		else if (isCompound)
		{

			// Iterate through children
			//
			unsigned int numChildren = plug.numChildren(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MPlug child;

			for (unsigned int i = 0; i < numChildren; i++)
			{

				child = plug.child(i, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				status = breakConnections(child, asDst, asSrc);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

		}
		else
		{

			// Check if incoming plugs should be broken
			//
			bool isConnected = plug.isConnected(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			if (asDst && isConnected)
			{

				MPlug otherPlug = plug.source(&status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				disconnectPlugs(otherPlug, plug);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

			// Check if outgoing plugs should be broken
			//
			MPlugArray otherPlugs = MPlugArray();
			bool hasConnections = plug.destinations(otherPlugs, &status);

			if (asSrc && hasConnections)
			{

				unsigned int numDestinations = otherPlugs.length();
				MPlug otherPlug;

				for (unsigned int i = 0; i < numDestinations; i++)
				{

					otherPlug = otherPlugs[i];

					disconnectPlugs(plug, otherPlug);
					CHECK_MSTATUS_AND_RETURN_IT(status);

				}

			}

		}

		return status;

	};

	MStatus connectPlugs(const MPlug& plug, const MPlug& otherPlug, const bool force)
	/**
	Connects the two supplied plugs.
	Using force will break any pre-existing connections.

	@param plug: The source plug.
	@param otherPlug: The destination plug.
	@param force: Breaks pre-existing connections.
	@return: Status code.
	*/
	{

		MStatus status;

		// Check if either plugs are null
		//
		bool isNull = plug.isNull(&status) || otherPlug.isNull(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isNull)
		{

			return MS::kFailure;

		}

		// Check if destination already has connections
		//
		bool isConnected = otherPlug.isConnected(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isConnected && force)
		{

			status = breakConnections(otherPlug, true, false);
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

		// Check if these are compound plugs
		//
		bool isCompound = plug.isCompound(&status) && otherPlug.isCompound(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isCompound)
		{

			// Iterate through children
			//
			unsigned int numChildren = plug.numChildren(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			for (unsigned int i = 0; i < numChildren; i++)
			{

				status = connectPlugs(plug.child(i), otherPlug.child(i), force);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

		}
		else
		{

			// Setup dag modifier
			//
			MDGModifier modifier;

			status = modifier.connect(plug, otherPlug);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Execute modifier
			//
			status = modifier.doIt();
			CHECK_MSTATUS_AND_RETURN_IT(status);

		}

		return status;

	};

	MStatus transferConnections(const MPlug& plug, const MPlug& otherPlug)
	/**
	Transfers the connections from one plug to another.

	@param plug: Plug to take connections from.
	@param otherPlug: Plug to pass connections to.
	@return: Status code.
	*/
	{

		MStatus status;

		// Check if either plugs are null
		//
		bool isNull = plug.isNull(&status) || otherPlug.isNull(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isNull)
		{

			return MS::kFailure;

		}
		
		// Check if either plugs are locked
		//
		bool isLocked = isPartiallyLocked(plug, &status) || isPartiallyLocked(otherPlug, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isLocked)
		{

			return MS::kSuccess;  // Nothing we can do from here!

		}

		// Check if these are compound plugs
		//
		bool isCompound = plug.isCompound(&status) && otherPlug.isCompound(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isCompound)
		{

			// Iterate through children
			//
			unsigned int numChildren = plug.numChildren(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			for (unsigned int i = 0; i < numChildren; i++)
			{

				status = transferConnections(plug.child(i), otherPlug.child(i));
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

		}
		else
		{

			// Disconnect plug from source
			//
			MPlug source = plug.source(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			isNull = source.isNull(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			if (!isNull)
			{

				status = disconnectPlugs(source, plug);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

			// Transfer connection to other plug
			//
			if (source != otherPlug && !isNull)
			{

				status = connectPlugs(source, otherPlug, true);
				CHECK_MSTATUS_AND_RETURN_IT(status);

			}

		}

		return status;

	};

	MStatus transferValues(MPlug& plug, MPlug& otherPlug)
	/**
	Transfers the values from one plug to another.

	@param plug: Plug to take values from.
	@param otherPlug: Plug to pass values to.
	@return: Status code.
	*/
	{
		
		MStatus status;

		// Check if either plugs are null
		//
		bool isNull = plug.isNull(&status) || otherPlug.isNull(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isNull)
		{

			return MS::kFailure;

		}

		// Check if either plugs are locked
		//
		bool isLocked = isPartiallyLocked(plug, &status) || isPartiallyLocked(otherPlug, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (isLocked)
		{

			return MS::kSuccess;  // Nothing we can do from here!

		}

		// Assign data handle to other plug
		//
		MDataHandle dataHandle = plug.asMDataHandle(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = otherPlug.setMDataHandle(dataHandle);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		plug.destructHandle(dataHandle);

		return status;

	};

	MStatus getAttributesByCategory(const MTypeId& id, const MString category, MObjectArray& attributes)
	/**
	Returns a list of attributes with the specified category from the associated type ID.

	@param id: The type ID to search from.
	@param category: The attribute category to filter.
	@param attributes: The passed array to populate.
	@return: Return status.
	*/
	{

		MStatus status;

		// Iterate through attributes
		//
		MNodeClass nodeClass = MNodeClass(id);
		unsigned int attributeCount = nodeClass.attributeCount();

		status = attributes.clear();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnAttribute fnAttribute;
		bool hasCategory;

		for (unsigned int i = 0; i < attributeCount; i++)
		{

			status = fnAttribute.setObject(nodeClass.attribute(i, &status));
			CHECK_MSTATUS_AND_RETURN_IT(status);

			hasCategory = fnAttribute.hasCategory(category);

			if (hasCategory)
			{

				attributes.append(fnAttribute.object());

			}
			else
			{

				continue;

			}

		}

		return status;

	};

	bool isSceneLoading()
	/**
	Evaluates if a scene file is being loaded.

	@return: Is loading.
	*/
	{
		
		return MFileIO::isNewingFile() || MFileIO::isOpeningFile() || MFileIO::isImportingFile() || MFileIO::isReferencingFile();

	};

	MVector getSceneUpVector()
	/**
	Returns the current scene up vector.

	@return: The up vector.
	*/
	{

		MStatus status;

		MVector upVector = MGlobal::upAxis(&status);
		CHECK_MSTATUS_AND_RETURN(status, MVector::yAxis);

		return upVector;

	};

};