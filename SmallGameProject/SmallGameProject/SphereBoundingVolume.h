#ifndef SPHEREBOUNDINGVOLUME_H
#define SPHEREBOUNDINGVOLUME_H

#include "BoundingVolume.h"
#include "BoxBoundingVolume.h"

class SphereBoundingVolume : public BoundingVolume {

private:
	DirectX::XMFLOAT3 center;
	float radius;

	void GenerateMinMax(DirectX::XMFLOAT3& minVertex, DirectX::XMFLOAT3& maxVertex, Model* model);

public:
	SphereBoundingVolume();
	virtual ~SphereBoundingVolume();
	void GenerateBounds(Model* model);
	bool Intersect(BoundingVolume* otherBoundingVolume);
	int getRadius();
	DirectX::XMFLOAT3 getCenter();
	bool SphereIntesectionTest(SphereBoundingVolume* otherSphere);
	bool BoxIntersectionTest(BoxBoundingVolume* box);
	void UpdateBoundingVolume(DirectX::XMMATRIX modelWorldMatrix);
};


#endif
