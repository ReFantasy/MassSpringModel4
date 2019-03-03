#ifndef __VIRTUALSTRESS_H__
#define __VIRTUALSTRESS_H__
#include "VECTOR3D.h"
#include "vtkheader.h"
class Layer;
using FORCE = VECTOR3D;

class VirtualStress
{
public:
	VirtualStress(Layer *layer1, Layer *layer2);
	FORCE GetForce(Layer *layer, int i, int j);

private:
	Layer *pLayer1 = nullptr;
	Layer *pLayer2 = nullptr;
};
#endif // __VIRTUALSTRESS_H__
