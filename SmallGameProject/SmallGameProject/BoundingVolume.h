#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

#include <DirectXMath.h>
#include "Model.h"

class BoundingVolume {

private:
	virtual void GenerateMinMax(DirectX::XMFLOAT3& minVert, DirectX::XMFLOAT3& maxVert, Model* model) = 0;


public:
	BoundingVolume();
	virtual ~BoundingVolume();

	virtual void GenerateBounds(Model* model) = 0;
	virtual bool Intersect(BoundingVolume* otherBoundingVolume) = 0;
	virtual void UpdateBoundingVolume(DirectX::XMMATRIX modelWorldMatrix) = 0;
};

#endif