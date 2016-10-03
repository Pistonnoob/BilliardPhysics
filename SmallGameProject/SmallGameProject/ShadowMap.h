#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include "CameraHandler.h"


class ShadowMap
{
protected:
	CameraHandler lightCamera;

public:
	ShadowMap();
	virtual ~ShadowMap();
};
#endif
