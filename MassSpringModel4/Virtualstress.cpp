#include "Virtualstress.h"
#include "Layer.h"
VirtualStress::VirtualStress(Layer *layer1, Layer *layer2)
	:pLayer1(layer1),pLayer2(layer2)
{

}

FORCE VirtualStress::GetForce(Layer *layer, int i, int j)
{
	FORCE force;
	float springLength = (pLayer1->GetMassRef(i, j).position - pLayer2->GetMassRef(i, j).position).GetLength();

	
	//伸展 = 当前长度 - 原始长度（符号包含了受力的方向）
	float extension = springLength - NaturalLengthDefault;

	//压力 = 弹性系数 * 伸展 / 自然长度
	double tension = SpringConstantDefault*extension / NaturalLengthDefault;

	if (layer == pLayer1)
	{
		VECTOR3D tensionDirection = pLayer2->GetMassRef(i, j).position - pLayer1->GetMassRef(i, j).position;
		tensionDirection.Normalize();
		force = tension*tensionDirection;
	}
	else if (layer == pLayer2)
	{
		VECTOR3D tensionDirection = pLayer1->GetMassRef(i, j).position - pLayer2->GetMassRef(i, j).position;
		tensionDirection.Normalize();
		force = tension*tensionDirection;
	}
	
	//return VECTOR3D(0, 0, 0);
	return force;
}


