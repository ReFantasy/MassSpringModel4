#ifndef __MASS_H__
#define __MASS_H__
#include "Maths.h"

class Mass
{
public:
	VECTOR3D position;
	VECTOR3D velocity;

	// ����
	float mass = 0.01;

	// �ʵ��Ƿ�̶�
	bool fixed = false;

	// �ʵ�ķ�����
	VECTOR3D normal;

	// ��ʼλ��
	VECTOR3D init_position;
};


#endif //__MASS_H__
