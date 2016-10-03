#include "BoxBoundingVolume.h"
#include "SphereBoundingVolume.h"
BoxBoundingVolume::BoxBoundingVolume()
{
	this->center = DirectX::XMFLOAT3(0,0,0);
	this->axisX = DirectX::XMVectorSet(0, 0, 0, 0);
	this->axisY = DirectX::XMVectorSet(0, 0, 0, 0);
	this->axisZ = DirectX::XMVectorSet(0, 0, 0, 0);

	for (int i = 0; i < 6; i++) {
		this->allAxises[i] = DirectX::XMVectorSet(0, 0, 0, 0);
	}

	this->halfLengthX = 0;
	this->halfLengthY = 0;
	this->halfLengthZ = 0;
	
	for (int i = 0; i < 3; i++) {
		this->allLength[i] = 0;
	}

	for (int i = 0; i < 8; i++) {
		this->vertices[i] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0,0,0));
	}

}

BoxBoundingVolume::~BoxBoundingVolume()
{
}

void BoxBoundingVolume::GenerateMinMax(DirectX::XMFLOAT3 & minVertex, DirectX::XMFLOAT3 & maxVertex, Model * model)
{
	DirectX::XMVECTOR vertVec;
	DirectX::XMFLOAT3 vertFloat;
	int vertCount = model->GetVertexCount();
	DirectX::XMFLOAT3 maxVert = DirectX::XMFLOAT3(-D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX);
	DirectX::XMFLOAT3 minVert = DirectX::XMFLOAT3(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);
	const std::vector<DirectX::XMFLOAT3>* vertPos = model->getVertexPositions();
	DirectX::XMMATRIX modelWorldMatrix;
	model->GetWorldMatrix(modelWorldMatrix);

	for (int i = 0; i < vertCount; i++) {

		vertVec = DirectX::XMLoadFloat3(&vertPos->at(i));
		vertVec = DirectX::XMVector3TransformCoord(vertVec, modelWorldMatrix);
		XMStoreFloat3(&vertFloat, vertVec);

		if (vertFloat.x > maxVert.x) {
			maxVert.x = vertFloat.x;
		}
		if (vertFloat.y > maxVert.y) {
			maxVert.y = vertFloat.y;
		}
		if (vertFloat.z > maxVert.z) {
			maxVert.z = vertFloat.z;
		}
		if (vertFloat.x < minVert.x) {
			minVert.x = vertFloat.x;
		}
		if (vertFloat.y < minVert.y) {
			minVert.y = vertFloat.y;
		}
		if (vertFloat.z < minVert.z) {
			minVert.z = vertFloat.z;
		}
	}

	minVertex = minVert;
	maxVertex = maxVert;

	vertPos = nullptr;
}

void BoxBoundingVolume::GenerateBounds(Model * model)
{
	DirectX::XMVECTOR minV;
	DirectX::XMVECTOR maxV;
	DirectX::XMFLOAT3 minVert;
	DirectX::XMFLOAT3 maxVert;
	DirectX::XMMATRIX world;
	model->GetWorldMatrix(world);
	
	this->GenerateMinMax(minVert, maxVert, model);
	
	//Move the values into vectors
	minV = DirectX::XMLoadFloat3(&minVert);
	maxV = DirectX::XMLoadFloat3(&maxVert);
	
	//Move the vertices to world space
	minV = DirectX::XMVector2TransformCoord(minV, world);
	maxV = DirectX::XMVector2TransformCoord(maxV, world);

	//Move back to a float3 for easier use
	DirectX::XMStoreFloat3(&minVert, minV);
	DirectX::XMStoreFloat3(&maxVert, maxV);

	//Calculate the midle
	DirectX::XMFLOAT3 distanceToMid = DirectX::XMFLOAT3((maxVert.x - minVert.x) / 2, (maxVert.y - minVert.y) / 2, (maxVert.z - minVert.z) / 2);	//Calculate offset to add to the min vertex to find the midle vertex
	DirectX::XMFLOAT3 midleVert = DirectX::XMFLOAT3(minVert.x + distanceToMid.x, minVert.y + distanceToMid.y, minVert.z + distanceToMid.z);	//Calculate the midle Vertex

	this->center = DirectX::XMFLOAT3(midleVert);	//Set the bounding spheres midle to the center of the modle
	
	//Half the length of the box
	this->allLength[0] = this->halfLengthX = distanceToMid.x;
	this->allLength[1] = this->halfLengthY = distanceToMid.y;
	this->allLength[2] = this->halfLengthZ = distanceToMid.z;

	//Calculate the axises
	this->axisX = DirectX::XMVectorSet(maxVert.x - minVert.x, 0, 0, 0);
	this->axisX = DirectX::XMVectorSet(0, (maxVert.y - minVert.y), 0, 0);
	this->axisX = DirectX::XMVectorSet(0, 0, (maxVert.z - minVert.z), 0);

	// Normalize the axises
	this->axisX = DirectX::XMVector3Normalize(this->axisX);
	this->axisY = DirectX::XMVector3Normalize(this->axisY);
	this->axisZ = DirectX::XMVector3Normalize(this->axisZ);
	
	//Save the axises and thier opposits in an array for easier use
	this->allAxises[0] = this->axisX;
	this->allAxises[1] = this->axisY;
	this->allAxises[2] = this->axisZ;
	this->allAxises[3] = DirectX::XMVectorSet(DirectX::XMVectorGetX(this->axisX) * -1, 0, 0, 0);
	this->allAxises[4] = DirectX::XMVectorSet(0, DirectX::XMVectorGetY(this->axisY) * -1, 0, 0);
	this->allAxises[5] = DirectX::XMVectorSet(0, 0, DirectX::XMVectorGetZ(this->axisZ) * -1, 0);

	//Calculate all the vertices	(y = 0 so since we dont need it)
	this->vertices[0] = DirectX::XMLoadFloat3(&minVert);
	this->vertices[1] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(maxVert.x, 0, minVert.z));
	this->vertices[2] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(maxVert.x, 0, maxVert.z));
	this->vertices[3] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(minVert.x, 0, minVert.z));
	this->vertices[4] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(minVert.x, 0, minVert.z));
	this->vertices[5] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(maxVert.x, 0, minVert.z));
	this->vertices[6] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(maxVert.x, 0, minVert.z));
	this->vertices[7] = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(minVert.x, 0, maxVert.z));

}

bool BoxBoundingVolume::Intersect(BoundingVolume * otherBoundingVolume)
{
	bool result = false;
	BoxBoundingVolume* box;

	SphereBoundingVolume* sphere = dynamic_cast<SphereBoundingVolume*>(otherBoundingVolume);
	if (sphere != nullptr) {	//If the volume is a sphere
		
		result = this->SphereIntersectionTest(sphere);
	}
	else{						//If the volume is a Box
		
		box = dynamic_cast<BoxBoundingVolume*>(otherBoundingVolume);

		result = this->BoxIntersectionTest(box);
	}

	sphere = nullptr;
	box = nullptr;

	return result;
}

bool BoxBoundingVolume::BoxIntersectionTest(BoxBoundingVolume* otherBox)
{
	DirectX::XMVECTOR* otherBoxAxsises = otherBox->getAxises();

	for (int i = 0; i < 6; i++) {
		
		if ( i != 1 || i != 4) {
			//Get the 2D axis since we will not check the y-axis
			DirectX::XMFLOAT3 twoDAxis = DirectX::XMFLOAT3(DirectX::XMVectorGetX(this->allAxises[i]), 0, DirectX::XMVectorGetX(this->allAxises[i]));
			DirectX::XMVECTOR axis = DirectX::XMLoadFloat3(&twoDAxis);

			DirectX::XMVECTOR p1 = this->Project(axis);
			DirectX::XMVECTOR p2 = otherBox->Project(axis);

			//if the is no overlap in one of the axises, the boxes does not intersect
			if (!this->Overlap(axis, p1, p2)) {
				return false;
			}
		}
	}

	otherBoxAxsises = nullptr;

	return true;
}

bool BoxBoundingVolume::SphereIntersectionTest(SphereBoundingVolume* sphere)
{
	bool result = false;

	DirectX::XMFLOAT3 sphereCenter = sphere->getCenter();
	float sR = sphere->getRadius();
	//bc = this center,
	//bu = x axsis,
	//be = length,
	//q = result

	DirectX::XMFLOAT3 closestPoint;

	//The vector from the center of the box to the center of the sphere
	DirectX::XMVECTOR v = DirectX::XMVectorSet(this->center.x - sphereCenter.x, this->center.y - sphereCenter.y, this->center.z - sphereCenter.z, 0);

	// Start result at center of box; make steps from there
	closestPoint = this->center;
	// For each OBB axis...
	for (int i = 0; i < 3; i++)
	{
		// ...project v onto that axis to get the distance
		// along the axis of v from the box center
		float dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v, this->allAxises[i]));
		// If distance farther than the box extents, clamp to the box
		float axisLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(this->allAxises[i]));

		if (dist > this->allLength[i] * axisLength) {
			dist = axisLength;
		}

		if (dist < -axisLength) {
			dist = -axisLength;
		}
		DirectX::XMFLOAT3 axisF;
		DirectX::XMStoreFloat3(&axisF, this->allAxises[i]);
		// Step that distance along the axis to get world coordinate
		closestPoint.x += dist * axisF.x;
		closestPoint.y += dist * axisF.y;
		closestPoint.z += dist * axisF.z;
	}

	//closest Point is know the closest point to the sphere center
	float distance = sqrt( pow((closestPoint.x - sphereCenter.x), 2) + pow((closestPoint.y - sphereCenter.y), 2) + pow((closestPoint.z - sphereCenter.z), 2));

	if (pow(distance,2) <= pow(sR,2)) {
		result = true;
	}

	return result;
}

DirectX::XMVECTOR* BoxBoundingVolume::getAxises()
{
	return this->allAxises;
}

float* BoxBoundingVolume::getLengths()
{
	return this->allLength;
}

const DirectX::XMFLOAT3 BoxBoundingVolume::getCenter()
{
	return this->center;
}

DirectX::XMVECTOR BoxBoundingVolume::Project(DirectX::XMVECTOR axis)
{
	// min is the smalest length to multiply the axis with to get the first point on the axsis for this shape
	// max  -||-
	float min = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, this->vertices[0]));
	float max = min;
	for (int i = 1; i < 8; i++) {
		// NOTE: the axis must be normalized to get accurate projections
		float p = DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis, this->vertices[i]));
		if (p < min) {
			min = p;
		}
		else if (p > max) {
			max = p;
		}
	}
	DirectX::XMVECTOR proj = DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(min, max));
	return proj;
}

bool BoxBoundingVolume::Overlap(DirectX::XMVECTOR axis, DirectX::XMVECTOR Vec1, DirectX::XMVECTOR Vec2)
{
	float v1Min = DirectX::XMVectorGetX(Vec1);
	float v1Max = DirectX::XMVectorGetY(Vec1);
	float v2Min = DirectX::XMVectorGetX(Vec2);
	float v2Max = DirectX::XMVectorGetY(Vec2);

	if ( v1Max >= v2Min) {	//if v1 max is longer than v2 min

		if (v1Min <= v2Max) {
			return true;
		}

	}

	return false;
}

void BoxBoundingVolume::UpdateBoundingVolume(DirectX::XMMATRIX modelWorldMatrix)
{
	this->axisX = DirectX::XMVector3TransformCoord(this->axisX, modelWorldMatrix);
	this->axisY = DirectX::XMVector3TransformCoord(this->axisY, modelWorldMatrix);
	this->axisZ = DirectX::XMVector3TransformCoord(this->axisZ, modelWorldMatrix);

	for (int i = 0; i < 6; i++) {
		this->allAxises[i] = DirectX::XMVector3TransformCoord(this->allAxises[i], modelWorldMatrix);
	}
	
	for (int i = 0; i < 8; i++) {
		this->vertices[i] = DirectX::XMVector3TransformCoord(this->vertices[i], modelWorldMatrix);
	}

}
