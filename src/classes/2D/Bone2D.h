#ifndef CLASS_BONE2D_H
#define CLASS_BONE2D_H

#include "variants.h"

struct Bone2D
{
	Bone2D(mat4 transform);

	Vec2 position();
	float rotation();

	mat4 transform;
};
#endif // CLASS_BONE2D_H
