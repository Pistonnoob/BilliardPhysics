#include "SphereBoundingVolume.h"

SphereBoundingVolume::SphereBoundingVolume()
: BoundingVolume()
{
	this->center = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	this->radius = 0;
}

SphereBoundingVolume::~SphereBoundingVolume()
{

}

void SphereBoundingVolume::GenerateMinMax(DirectX::XMFLOAT3& minVertex, DirectX::XMFLOAT3& maxVertex, Model* model)
{
	DirectX::XMVECTOR vertVec;
	DirectX::XMFLOAT3 vertFloat;
	int vertCount = model->GetVertexCount();
	DirectX::XMFLOAT3 maxVert = DirectX::XMFLOAT3(-D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX);
	DirectX::XMFLOAT3 minVert = DirectX::XMFLOAT3(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);
	const std::vector<DirectX::XMFLOAT3>* vertPos = model->getVertexPositions();

	for (int i = 0; i < vertCount; i++) {

		vertVec = DirectX::XMLoadFloat3(&vertPos->at(i));
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

void SphereBoundingVolume::GenerateBounds(Model* model)
{
	DirectX::XMVECTOR minV;
	DirectX::XMVECTOR maxV;
	DirectX::XMFLOAT3 minVert;
	DirectX::XMFLOAT3 maxVert;
	DirectX::XMMATRIX world;
	model->GetWorldMatrix(world);

	this->GenerateMinMax(minVert, maxVert, model);	// Generate the min max based on the model

	//Move the values into vectors
	minV = DirectX::XMLoadFloat3(&minVert);
	maxV = DirectX::XMLoadFloat3(&maxVert);

	//Move the vertices to world space
	//minV = DirectX::XMVector3TransformCoord(minV, world);
	//maxV = DirectX::XMVector3TransformCoord(maxV, world);

	//Move back to a float3 for easier use
	DirectX::XMStoreFloat3(&minVert, minV);
	DirectX::XMStoreFloat3(&maxVert, maxV);
	 
	DirectX::XMFLOAT3 midleVert = DirectX::XMFLOAT3(minVert.x + ((maxVert.x - minVert.x) / 2), minVert.y + ((maxVert.y - minVert.y) / 2), minVert.z + ((maxVert.z - minVert.z) / 2));	//Calculate the midle Vertex

	this->center = DirectX::XMFLOAT3(midleVert);	//Set the bounding spheres midle to the center of the modle
	this->radius = abs(midleVert.x - minVert.x);	//Set the radius of the sphere to the distance from the midle to the minimum vertex

}

bool SphereBoundingVolume::Intersect(BoundingVolume* otherBoundingVolume)
{
	bool result = false;
	BoxBoundingVolume* box;
	SphereBoundingVolume* sphere = dynamic_cast<SphereBoundingVolume*>(otherBoundingVolume);
	if (sphere != nullptr) {	//If the volume is a sphere
		
		result = this->SphereIntesectionTest(sphere);		
	}
	else {						//If the volume is a box
		box = dynamic_cast<BoxBoundingVolume*>(otherBoundingVolume);

		result = this->BoxIntersectionTest(box);
	}
	
	sphere = nullptr;
	box = nullptr;

	return result;
}

int SphereBoundingVolume::getRadius()
{
	return this->radius;
}

DirectX::XMFLOAT3 SphereBoundingVolume::getCenter()
{
	return this->center;
}

bool SphereBoundingVolume::SphereIntesectionTest(SphereBoundingVolume* otherSphere)
{
	bool result = false;
	//The distance betwen the two centers
	float distance = sqrt(pow((this->center.x - otherSphere->center.x), 2) + pow((this->center.y - otherSphere->center.y), 2) + pow((this->center.z - otherSphere->center.z), 2));

	if (distance <= this->radius + otherSphere->radius) {
		result = true;
	}

	return result;
}

bool SphereBoundingVolume::BoxIntersectionTest(BoxBoundingVolume* box)
{
	bool result = false;

	DirectX::XMFLOAT3 boxCenter = box->getCenter();
	float* allLengths = box->getLengths();
	DirectX::XMVECTOR* allAxises = box->getAxises();

	DirectX::XMFLOAT3 closestPoint;

	//The vector from the center of the box to the center of the sphere
	DirectX::XMVECTOR v = DirectX::XMVectorSet(boxCenter.x - this->center.x, boxCenter.y - this->center.y, boxCenter.z - this->center.z, 0);

	// Start result at center of box; make steps from there
	closestPoint = boxCenter;
	// For each OBB axis...
	for (int i = 0; i < 3; i++)
	{
		// ...project v onto that axis to get the distance
		// along the axis of v from the box center
		float dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v, allAxises[i]));
		// If distance farther than the box extents, clamp to the box
		float axisLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(allAxises[i]));

		if (dist > allLengths[i] * axisLength) {
			dist = axisLength;
		}

		if (dist < -axisLength) {
			dist = -axisLength;
		}
		DirectX::XMFLOAT3 axisF;
		DirectX::XMStoreFloat3(&axisF, allAxises[i]);
		// Step that distance along the axis to get world coordinate
		closestPoint.x += dist * axisF.x;
		closestPoint.y += dist * axisF.y;
		closestPoint.z += dist * axisF.z;
	}

	//closest Point is know the closest point to the sphere center
	float distance = sqrt(pow((closestPoint.x - this->center.x), 2) + pow((closestPoint.y - this->center.y), 2) + pow((closestPoint.z - this->center.z), 2));

	if (pow(distance, 2) <= pow(this->radius, 2)) {
		result = true;
	}

	allLengths = nullptr;
	allAxises = nullptr;

	return result;
}

void SphereBoundingVolume::UpdateBoundingVolume(DirectX::XMMATRIX modelWorldMatrix)
{
	DirectX::XMVECTOR pos = DirectX::XMVectorSet(0, 1, 0, 1);	// local pos of model and 1 in y
	pos = DirectX::XMVector3TransformCoord(pos, modelWorldMatrix);
	DirectX::XMStoreFloat3(&this->center, pos);
}
