#ifndef BOXBOUNDINGVOLUME_H
#define BOXBOUNDINGVOLUME_H

#include "BoundingVolume.h"

class SphereBoundingVolume;

class BoxBoundingVolume : public BoundingVolume {

private:
	DirectX::XMFLOAT3 center;

	//The axices of the OBB
	DirectX::XMVECTOR axisX;
	DirectX::XMVECTOR axisY;
	DirectX::XMVECTOR axisZ;

	DirectX::XMVECTOR allAxises[6]; // first 3 = x,y,z  Last 3 = -x, -y, -z

	// Half the length of one side of the OBB
	float halfLengthX;
	float halfLengthY;
	float halfLengthZ;
	float allLength[3];

	DirectX::XMVECTOR vertices[8];

	void GenerateMinMax(DirectX::XMFLOAT3& minVertex, DirectX::XMFLOAT3& maxVertex, Model* model);
	bool BoxIntersectionTest(BoxBoundingVolume* otherBox);
	bool SphereIntersectionTest(SphereBoundingVolume* sphere);
	
public:
	BoxBoundingVolume();
	virtual ~BoxBoundingVolume();
	void GenerateBounds(Model* model);
	bool Intersect(BoundingVolume* otherBoundingVolume);
	DirectX::XMVECTOR* getAxises();
	float* getLengths();
	const DirectX::XMFLOAT3 getCenter();
	DirectX::XMVECTOR Project(DirectX::XMVECTOR axis);
	bool Overlap(DirectX::XMVECTOR axis, DirectX::XMVECTOR Vec1, DirectX::XMVECTOR Vec2);
	void UpdateBoundingVolume(DirectX::XMMATRIX modelWorldMatrix);
};


#endif
