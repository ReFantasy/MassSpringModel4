#include "Layer.h"
#include "Virtualstress.h"
// 重力
VECTOR3D gravity(0.0f, -0.0f, 0.0f);

// 弹簧属性
float SpringConstantDefault = 15.f;
float NaturalLengthDefault = 1.0f;

// 质点质量
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
	//计算弹簧张量
	for (auto it = springs.begin(); it != springs.end(); it++)
	{
		//弹簧长度 = 弹簧两端点坐标距离
		float springLength =
			((*pCurrent)[it->second.imass1].position - (*pCurrent)[it->second.imass2].position).GetLength();
		//cout << "len:" << springLength << endl;

		//伸展 = 当前长度 - 原始长度（符号包含了受力的方向）
		float extension = springLength - it->second.naturalLength;

		//压力 = 弹性系数 * 伸展 / 自然长度
		it->second.tension = it->second.springConstant*extension / it->second.naturalLength;
	}

	// 遍历每个质点 并更新其位置
	for (int i = 0; i < grid_size*grid_size; i++)
	{
		// 不改变的属性
		(*pNext)[i].fixed = (*pCurrent)[i].fixed;
		(*pNext)[i].mass = (*pCurrent)[i].mass;

		// 如果球的位置固定，则位置不变，速度为零，否则更新信息
		if ((*pCurrent)[i].fixed)
		{
			(*pNext)[i].position = (*pCurrent)[i].position;
			(*pNext)[i].velocity.LoadZero();
		}
		else
		{
			// 计算质点受力
			VECTOR3D force = gravity;
			// 查找该质点所连接的弹簧，累加其所受到的弹簧力
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

			// 阻尼力
			//force += nextBalls[i].velocity*0.1;
			if (i == grid_size*grid_size / 2)
			{
				force += VECTOR3D(0, -20.0, 0);
			}
			// 虚拟应力
			if (pVirtualStress)
			{
				vtkIdType tmpi = 0, tmpj = 0;
				GetMassCoord(i, tmpi, tmpj);
				force += pVirtualStress->GetForce(this, tmpi, tmpj);
			}
			// 加速度
			VECTOR3D acceleration = force / (*pCurrent)[i].mass;

			// 更新速度
			(*pNext)[i].velocity = (*pCurrent)[i].velocity + acceleration*(float)timePassedInSeconds;

			//Damp the velocity
			(*pNext)[i].velocity *= dampFactor; //防止当加速度为0 但是速度不为零时出现无法停止的情况

												 // 更新位置
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

	// 设置纹理st坐标系的顶点坐标 和两点坐标，定义了st坐标系
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
	// 初始化质点
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
	// 初始化结构弹簧 y方向
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
	// 初始化结构弹簧 x方向
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

	//剪切弹簧初始化-右下
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

	//剪切弹簧初始化-左下
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

	//弯曲弹簧初始化
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
			// 沿x方向
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
			// 沿y方向
			vtkSmartPointer<vtkLine> line_y = vtkSmartPointer<vtkLine>::New();
			line_y->GetPointIds()->SetId(0, i*grid_size + j);
			line_y->GetPointIds()->SetId(1, i*grid_size + j + 1);
			springs_structure->InsertNextCell(line_y);
		}
	}
	// 加入四边形拓扑结构vtkQuadraticQuad 
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

