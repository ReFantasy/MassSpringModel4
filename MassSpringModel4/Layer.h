#ifndef __LAYER_H__
#define __LAYER_H__
#include "vtkheader.h"
#include "Mass.h"
#include "Spring.h"
#include <vector>
#include <map>
using namespace std;
// ����
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
private:  // ����

	int grid_size;             // ����������߳�
	vector<Mass> masses1;       // �ʵ�����
	vector<Mass> masses2;       // �ʵ�����
	vector<Mass> *pCurrent = nullptr;
	vector<Mass> *pNext = nullptr;

	int layer_height = 0;  // �ʵ㵯�ɲ�ĸ߶�

						   // ��������
	float SpringConstantDefault = 25.f;
	float NaturalLengthDefault = 1.0f;

	// �ʵ�����
	float MassDefault = 0.01f;

	//Damping factor. Velocities are multiplied by this
	float dampFactor = 0.5f;


	// ����map  ����Ϊ���������ʵ��ID
	map<pair<vtkIdType, vtkIdType>, Spring> springs;    // ��������

														//vtk���ӻ�
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();                 // �����
	vtkSmartPointer<vtkCellArray> springs_structure = vtkSmartPointer<vtkCellArray>::New();    // �ṹ����
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();

	VirtualStress *pVirtualStress = nullptr;

	// ��������
	// �����ݵı���ֵ
	vtkSmartPointer<vtkDoubleArray> weights = vtkSmartPointer<vtkDoubleArray >::New();
	// �������εķ�����
	vtkSmartPointer<vtkPolyDataNormals > normal = vtkSmartPointer<vtkPolyDataNormals >::New();
	// ���ö���ε�����ӳ��ģʽ
	// ����Ϊplaneģʽ
	vtkSmartPointer<vtkTextureMapToPlane> tmapper = vtkSmartPointer<vtkTextureMapToPlane>::New();
	// ������ӳ����м���
	vtkSmartPointer<vtkTransformTextureCoords> xform = vtkSmartPointer<vtkTransformTextureCoords>::New();
	// ��ȡ����ͼ��
	vtkSmartPointer<vtkJPEGReader> bmpReader = vtkSmartPointer<vtkJPEGReader>::New();
	vtkSmartPointer<vtkTexture> atext = vtkSmartPointer<vtkTexture>::New();

	// ���ӻ�
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
private:  // ˽�к���
	void InitMass();
	void InitSpring();
	void InitVtkData();
	// ������������(i,j)��ȡID
	vtkIdType GetMassId(vtkIdType i, vtkIdType j)
	{
		return i*grid_size + j;
	}

	// ��������ID��ȡ����(i,j)
	void GetMassCoord(vtkIdType id, vtkIdType &i, vtkIdType &j)
	{
		i = id / grid_size;
		j = id % grid_size;
	}

	void UpdateVtkData(vector<Mass> *pCurrent);
};
#endif //__LAYER_H__
