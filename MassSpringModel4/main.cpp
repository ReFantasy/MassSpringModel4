#include <iostream>
#include "vtkheader.h"
#include "Layer.h"
#include "MyInteractorStyle.h"
#include "TIMER.h"
#include "Virtualstress.h"
#include <windows.h>
TIMER timer;
//vtk���ӻ�
auto renderer = vtkSmartPointer<vtkRenderer>::New();
auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

// ����֡
void UpdateFrame();
int grid_size = 27;
Layer layer(grid_size);
Layer layer2(grid_size, 1);
VirtualStress virtualStress(&layer, &layer2);


int main()
{
	layer.SetVirtualStress(&virtualStress);
	layer2.SetVirtualStress(&virtualStress);

	// �̶����еı�Ե
	for (int i = 0; i < grid_size; i++)
	{
		layer.GetMassRef(i).fixed = true;
		layer.GetMassRef(i*grid_size).fixed = true;
		layer.GetMassRef((i + 1)*grid_size - 1).fixed = true;
		layer.GetMassRef(grid_size*grid_size - 1 - i).fixed = true;

		layer2.GetMassRef(i).fixed = true;
		layer2.GetMassRef(i*grid_size).fixed = true;
		layer2.GetMassRef((i + 1)*grid_size - 1).fixed = true;
		layer2.GetMassRef(grid_size*grid_size - 1 - i).fixed = true;
	}
	for (int i = 0; i < grid_size*grid_size; i++)
	{
		layer.GetMassRef(i).fixed = true;
	}
	
	//layer.SetTexture("./ImageResource/1.jpg");
	//layer2.SetTexture("./ImageResource/3.jpg");
	
	//renderer->AddActor(layer.GetActor());
	renderer->AddActor(layer2.GetActor());

	// ����Ⱦ�������ò���ʾ����ϵ
	vtkSmartPointer<vtkAxesActor> axes1 = vtkSmartPointer<vtkAxesActor>::New();
	axes1->SetShaftTypeToLine();
	axes1->SetConeRadius(0.1);
	axes1->SetConeResolution(100);
	axes1->SetTotalLength(7, 7, 3);
	
	renderer->AddActor(axes1);


	renderWindow->AddRenderer(renderer);

	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindow->SetSize(800,600);
	renderWindow->Render();

	// ��ʱ��
	vtkSmartPointer<MyInteractorStyle> style = vtkSmartPointer<MyInteractorStyle>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	style->RegisterOnTimerFunc(UpdateFrame);

	style->SetUseTimers(30);
	style->UseTimersOn();

	//while (1)
	//{
	//	Sleep(50);
	//	UpdateFrame();
	//	renderWindow->Render();
	//}


	//style->SetTimerDuration(30);
	//style->StartTimer();


	renderWindowInteractor->Start();

	system("pause");
	return 0;
}
void UpdateFrame()
{
	// ���õ�ǰʱ���Լ�������ʱ��
	static double lastTime = timer.GetTime();

	double currentTime = timer.GetTime();
	double timePassed = currentTime - lastTime;
	lastTime = currentTime;

	// ÿ��10�������һ������λ�ã��Է�ֹ��ͬ֡���ʵ��²�ͬ��������⡣
	static double timeSinceLastUpdate = 0.0;
	timeSinceLastUpdate += timePassed;
	while (timeSinceLastUpdate > 10.0)
	{
		timeSinceLastUpdate -= 10.0;
		float timePassedInSeconds = 0.01f;
		layer.UpdateFrame(timePassedInSeconds);
		layer2.UpdateFrame(timePassedInSeconds);
		//layer2.PrintInfo();
	}
	
	//renderWindow->Render();
}




