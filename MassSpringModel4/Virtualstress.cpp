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

	
	//��չ = ��ǰ���� - ԭʼ���ȣ����Ű����������ķ���
	float extension = springLength - NaturalLengthDefault;

	//ѹ�� = ����ϵ�� * ��չ / ��Ȼ����
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


