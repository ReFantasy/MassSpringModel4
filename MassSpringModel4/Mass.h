#ifndef __MASS_H__
#define __MASS_H__
#include "Maths.h"

class Mass
{
public:
	VECTOR3D position;
	VECTOR3D velocity;

	// 质量
	float mass = 0.01;

	// 质点是否固定
	bool fixed = false;

	// 质点的法向量
	VECTOR3D normal;

	// 初始位置
	VECTOR3D init_position;
};


#endif //__MASS_H__
