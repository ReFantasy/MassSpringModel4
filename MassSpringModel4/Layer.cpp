#include "Layer.h"
#include "Virtualstress.h"
// ����
VECTOR3D gravity(0.0f, -0.0f, 0.0f);

// ��������
float SpringConstantDefault = 15.f;
float NaturalLengthDefault = 1.0f;

// �ʵ�����
float MassDefault = 0.01f;

//Damping factor. Velocities are multiplied by this
float dampFactor = 0.1f;

Layer::Layer(int grid_sz, double y)
	:grid_size(grid_sz), layer_height(y)
{
	InitMass();
	InitSpring();
	InitVtkData();
}

bool Layer::SetMassFixed(vtkIdType i, vtkIdType j, bool is_fixed)
{
	vtkIdType id = GetMassId(i, j);
	masses1[id].fixed = is_fixed;
	masses2[id].fixed = is_fixed;
	return true;
}

void Layer::UpdateFrame(float timePassedInSeconds)
{
	//���㵯������
	for (auto it = springs.begin(); it != springs.end(); it++)
	{
		//���ɳ��� = �������˵��������
		float springLength =
			((*pCurrent)[it->second.imass1].position - (*pCurrent)[it->second.imass2].position).GetLength();
		//cout << "len:" << springLength << endl;

		//��չ = ��ǰ���� - ԭʼ���ȣ����Ű����������ķ���
		float extension = springLength - it->second.naturalLength;

		//ѹ�� = ����ϵ�� * ��չ / ��Ȼ����
		it->second.tension = it->second.springConstant*extension / it->second.naturalLength;
	}

	// ����ÿ���ʵ� ��������λ��
	for (int i = 0; i < grid_size*grid_size; i++)
	{
		// ���ı������
		(*pNext)[i].fixed = (*pCurrent)[i].fixed;
		(*pNext)[i].mass = (*pCurrent)[i].mass;

		// ������λ�ù̶�����λ�ò��䣬�ٶ�Ϊ�㣬���������Ϣ
		if ((*pCurrent)[i].fixed)
		{
			(*pNext)[i].position = (*pCurrent)[i].position;
			(*pNext)[i].velocity.LoadZero();
		}
		else
		{
			// �����ʵ�����
			VECTOR3D force = gravity;
			// ���Ҹ��ʵ������ӵĵ��ɣ��ۼ������ܵ��ĵ�����
			for (auto it = springs.begin(); it != springs.end(); it++)
			{
				if (it->second.imass1 == i)
				{
					VECTOR3D tensionDirection = (*pCurrent)[it->second.imass2].position - (*pCurrent)[it->second.imass1].position;
					tensionDirection.Normalize();

					force += it->second.tension*tensionDirection;
				}
				//Similarly if the imass is "imass2"
				if (it->second.imass2 == i)
				{
					VECTOR3D tensionDirection = (*pCurrent)[it->second.imass1].position - (*pCurrent)[it->second.imass2].position;
					tensionDirection.Normalize();

					force += it->second.tension*tensionDirection;
				}
			}

			// ������
			//force += nextBalls[i].velocity*0.1;
			if (i == grid_size*grid_size / 2)
			{
				force += VECTOR3D(0, -20.0, 0);
			}
			// ����Ӧ��
			if (pVirtualStress)
			{
				vtkIdType tmpi = 0, tmpj = 0;
				GetMassCoord(i, tmpi, tmpj);
				force += pVirtualStress->GetForce(this, tmpi, tmpj);
			}
			// ���ٶ�
			VECTOR3D acceleration = force / (*pCurrent)[i].mass;

			// �����ٶ�
			(*pNext)[i].velocity = (*pCurrent)[i].velocity + acceleration*(float)timePassedInSeconds;

			//Damp the velocity
			(*pNext)[i].velocity *= dampFactor; //��ֹ�����ٶ�Ϊ0 �����ٶȲ�Ϊ��ʱ�����޷�ֹͣ�����

												 // ����λ��
			(*pNext)[i].position = (*pCurrent)[i].position +
				((*pNext)[i].velocity + (*pCurrent)[i].velocity)*(float)timePassedInSeconds / 2;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//*pCurrent = *pNext;
	vector<Mass> *temp = pCurrent;
	pCurrent = pNext;
	pNext = temp;

	UpdateVtkData(pCurrent);
}

void Layer::PrintInfo()
{
	cout << "Print Mass Info..." << endl;
	for (int id = 0; id < masses1.size(); id++)
	{
		vtkIdType i, j;
		GetMassCoord(id, i, j);
		cout << "Id:" << id << " i:" << i << " j:" << j << endl;
	}

	cout << "Print Spring Info..." << endl;
	for (auto &e : springs)
	{
		vtkIdType p1, p2;
		p1 = e.first.first;
		p2 = e.first.second;
		vtkIdType i_1, j_1, i_2, j_2;
		GetMassCoord(p1, i_1, j_1);
		GetMassCoord(p2, i_2, j_2);
		printf("(%lld,%lld)->(%lld,%lld)\n", i_1, j_1, i_2, j_2);
	}
	
}

vtkSmartPointer<vtkTexture> Layer::SetTexture(std::string fileName)
{
	
	normal->SetInputData(polydata);

	
	tmapper->SetInputConnection(normal->GetOutputPort());

	// ��������st����ϵ�Ķ������� ���������꣬������st����ϵ
	tmapper->SetOrigin(0, 0, 0);
	tmapper->SetPoint1(100, 100, 0);
	tmapper->SetPoint2(0, 0, 11);

	
	xform->SetInputConnection(tmapper->GetOutputPort());
	xform->SetScale(1, 1, 1);

	
	bmpReader->SetFileName(fileName.c_str());

	atext->SetInputConnection(bmpReader->GetOutputPort());
	atext->InterpolateOn();

	mapper->SetInputConnection(xform->GetOutputPort());
	actor->SetTexture(atext);
	return atext;
}

void Layer::InitMass()
{
	int y = layer_height;
	// ��ʼ���ʵ�
	for (vtkIdType i = 0; i < grid_size; i++)
	{
		for (vtkIdType j = 0; j < grid_size; j++)
		{
			Mass mass;
			mass.fixed = false;
			mass.mass = 0.01;
			mass.position.Set(i, y, j);
			mass.velocity.Set(0, 0, 0);
			masses1.push_back(mass);
		}
	}
	masses2 = masses1;
	pCurrent = &masses1;
	pNext = &masses2;
}

void Layer::InitSpring()
{
	// ��ʼ���ṹ���� y����
	for (vtkIdType i = 0; i < grid_size; i++)
	{
		for (vtkIdType j = 0; j < grid_size - 1; j++)
		{
			Spring spring;
			spring.imass1 = GetMassId(i, j);
			spring.imass2 = GetMassId(i, j + 1);
			spring.naturalLength = NaturalLengthDefault;
			spring.springConstant = SpringConstantDefault;
			spring.tension = 0;
			springs[{ GetMassId(i, j), GetMassId(i, j + 1) }] = spring;
			//springs.insert({ GetMassId(i, j) ,GetMassId(i, j + 1) }, spring);
		}
	}
	// ��ʼ���ṹ���� x����
	for (int i = 0; i < grid_size - 1; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			Spring spring;
			spring.imass1 = GetMassId(i, j);
			spring.imass2 = GetMassId(i + 1, j);
			spring.naturalLength = NaturalLengthDefault;
			spring.springConstant = SpringConstantDefault;
			spring.tension = 0;
			springs[{ GetMassId(i, j), GetMassId(i + 1, j) }] = spring;
			//springs.insert({ GetMassId(i, j) ,GetMassId(i + 1, j) }, spring);
		}
	}

	//���е��ɳ�ʼ��-����
	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and right
	//excluding those on the bottom or right
	for (int i = 0; i < grid_size - 1; ++i)
	{
		for (int j = 0; j < grid_size - 1; ++j)
		{
			Spring spring;
			spring.imass1 = GetMassId(i, j);
			spring.imass2 = GetMassId(i + 1, j + 1);
			spring.naturalLength = NaturalLengthDefault*sqrt(2.0);
			spring.springConstant = SpringConstantDefault;
			spring.tension = 0;
			springs[{ GetMassId(i, j), GetMassId(i + 1, j + 1) }] = spring;
		}
	}

	//���е��ɳ�ʼ��-����
	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and left
	//excluding those on the bottom or right
	for (int i = 0; i < grid_size - 1; ++i)
	{
		for (int j = 1; j < grid_size; ++j)
		{
			Spring spring;
			spring.imass1 = GetMassId(i, j);
			spring.imass2 = GetMassId(i + 1, j - 1);
			spring.naturalLength = NaturalLengthDefault*sqrt(2.0);
			spring.springConstant = SpringConstantDefault;
			spring.tension = 0;
			springs[{ GetMassId(i, j), GetMassId(i + 1, j - 1) }] = spring;
		}
	}

	//�������ɳ�ʼ��
	//The first (gridSize-2)*gridSize springs go from one ball to the next but one,
	//excluding those on or next to the right hand edge
	for (int i = 0; i < grid_size; ++i)
	{
		for (int j = 0; j < grid_size - 2; ++j)
		{
			Spring spring;
			spring.imass1 = GetMassId(i, j);
			spring.imass2 = GetMassId(i, j + 2);
			spring.naturalLength = NaturalLengthDefault * 2;
			spring.springConstant = SpringConstantDefault;
			spring.tension = 0;
			springs[{ GetMassId(i, j), GetMassId(i, j + 2) }] = spring;
		}
	}

	//The next (gridSize-2)*gridSize springs go from one ball to the next but one below,
	//excluding those on or next to the bottom edge
	for (int i = 0; i < grid_size - 2; ++i)
	{
		for (int j = 0; j < grid_size; ++j)
		{
			Spring spring;
			spring.imass1 = GetMassId(i, j);
			spring.imass2 = GetMassId(i + 2, j);
			spring.naturalLength = NaturalLengthDefault * 2;
			spring.springConstant = SpringConstantDefault;
			spring.tension = 0;
			springs[{ GetMassId(i, j), GetMassId(i + 2, j) }] = spring;
		}
	}
}

void Layer::InitVtkData()
{
	int y = layer_height;
	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			points->InsertNextPoint(i, y, j);
		}
	}

	for (int i = 0; i < grid_size - 1; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			// ��x����
			vtkSmartPointer<vtkLine> line_x = vtkSmartPointer<vtkLine>::New();
			line_x->GetPointIds()->SetId(0, i*grid_size + j);
			line_x->GetPointIds()->SetId(1, (i + 1)*grid_size + j);
			springs_structure->InsertNextCell(line_x);
		}
	}
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size - 1; j++)
		{
			// ��y����
			vtkSmartPointer<vtkLine> line_y = vtkSmartPointer<vtkLine>::New();
			line_y->GetPointIds()->SetId(0, i*grid_size + j);
			line_y->GetPointIds()->SetId(1, i*grid_size + j + 1);
			springs_structure->InsertNextCell(line_y);
		}
	}
	// �����ı������˽ṹvtkQuadraticQuad 
	vtkSmartPointer<vtkCellArray> quad = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < grid_size - 1; i++)
	{
		for (int j = 0; j < grid_size - 1; j++)
		{
			quad->InsertNextCell(4);
			quad->InsertCellPoint(GetMassId(i, j));
			quad->InsertCellPoint(GetMassId(i, j + 1));
			quad->InsertCellPoint(GetMassId(i + 1, j + 1));
			quad->InsertCellPoint(GetMassId(i + 1, j));
		}
	}


	polydata->SetPoints(points);
	polydata->SetLines(springs_structure);
	polydata->SetPolys(quad);

	mapper->SetInputData(polydata);
	actor->SetMapper(mapper);
}

void Layer::UpdateVtkData(vector<Mass> *pCurrent)
{
	for (int i = 0; i < grid_size; ++i)
	{
		for (int j = 0; j < grid_size; ++j)
		{
			points->SetPoint(
				i*grid_size + j,
				(*pCurrent)[i*grid_size + j].position.x,
				(*pCurrent)[i*grid_size + j].position.y,
				(*pCurrent)[i*grid_size + j].position.z);
		}
	}

	points->Modified();
}

