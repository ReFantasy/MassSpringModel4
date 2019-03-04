#ifndef __LAYER_H__
#define __LAYER_H__
#include "vtkheader.h"
#include "Mass.h"
#include "Spring.h"
#include <vector>
#include <map>
using namespace std;
// 重力
extern VECTOR3D gravity;

class VirtualStress;
class Layer
{
public:
	Layer(int grid_sz, double y = 0);
	bool SetMassFixed(vtkIdType i, vtkIdType j, bool is_fixed);
	void UpdateFrame(float timePassedInSeconds);
	void PrintInfo();
	Mass & GetMassRef(int i, int j)
	{
		return (*pCurrent)[GetMassId(i, j)];
	}
	Mass & GetMassRef(vtkIdType id) 
	{ 
		return (*pCurrent)[id];
	}
	vtkSmartPointer<vtkPolyData> GetPolydata() { return polydata; }

	void SetVirtualStress(VirtualStress *_pVirtualStress) { pVirtualStress = _pVirtualStress; }

	vtkSmartPointer<vtkTexture> SetTexture(std::string fileName);
	vtkSmartPointer<vtkActor> GetActor() const { return actor; }
private:  // 数据

	int grid_size;             // 正方形网格边长
	vector<Mass> masses1;       // 质点向量
	vector<Mass> masses2;       // 质点向量
	vector<Mass> *pCurrent = nullptr;
	vector<Mass> *pNext = nullptr;

	int layer_height = 0;  // 质点弹簧层的高度

						   // 弹簧属性
	float SpringConstantDefault = 25.f;
	float NaturalLengthDefault = 1.0f;

	// 质点质量
	float MassDefault = 0.01f;

	//Damping factor. Velocities are multiplied by this
	float dampFactor = 0.5f;


	// 弹簧map  索引为弹簧两端质点的ID
	map<pair<vtkIdType, vtkIdType>, Spring> springs;    // 弹簧向量

														//vtk可视化
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();                 // 网格点
	vtkSmartPointer<vtkCellArray> springs_structure = vtkSmartPointer<vtkCellArray>::New();    // 结构弹簧
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();

	VirtualStress *pVirtualStress = nullptr;

	// 纹理数据
	// 点数据的标量值
	vtkSmartPointer<vtkDoubleArray> weights = vtkSmartPointer<vtkDoubleArray >::New();
	// 计算多边形的法向量
	vtkSmartPointer<vtkPolyDataNormals > normal = vtkSmartPointer<vtkPolyDataNormals >::New();
	// 设置多边形的纹理映射模式
	// 设置为plane模式
	vtkSmartPointer<vtkTextureMapToPlane> tmapper = vtkSmartPointer<vtkTextureMapToPlane>::New();
	// 对纹理映射进行计算
	vtkSmartPointer<vtkTransformTextureCoords> xform = vtkSmartPointer<vtkTransformTextureCoords>::New();
	// 读取纹理图像
	vtkSmartPointer<vtkJPEGReader> bmpReader = vtkSmartPointer<vtkJPEGReader>::New();
	vtkSmartPointer<vtkTexture> atext = vtkSmartPointer<vtkTexture>::New();

	// 可视化
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
private:  // 私有函数
	void InitMass();
	void InitSpring();
	void InitVtkData();
	// 根据网格坐标(i,j)获取ID
	vtkIdType GetMassId(vtkIdType i, vtkIdType j)
	{
		return i*grid_size + j;
	}

	// 根据网格ID获取坐标(i,j)
	void GetMassCoord(vtkIdType id, vtkIdType &i, vtkIdType &j)
	{
		i = id / grid_size;
		j = id % grid_size;
	}

	void UpdateVtkData(vector<Mass> *pCurrent);
};
#endif //__LAYER_H__
