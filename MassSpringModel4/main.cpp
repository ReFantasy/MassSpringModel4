#include <iostream>
#include "vtkheader.h"
#include "Layer.h"
#include "MyInteractorStyle.h"
#include "TIMER.h"
#include "Virtualstress.h"
#include <windows.h>
TIMER timer;
//vtk可视化
auto renderer = vtkSmartPointer<vtkRenderer>::New();
auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

// 更新帧
void UpdateFrame();
int grid_size = 17;
Layer layer0(grid_size,0,0);
Layer layer1(grid_size, 1,1);
Layer layer2(grid_size, 2,2);
Layer layer3(grid_size, 3, 3);
VirtualStress virtualStress01(&layer0, &layer1);
VirtualStress virtualStress12(&layer1, &layer2);
VirtualStress virtualStress23(&layer1, &layer2);


int main()
{
	layer0.SetVirtualStress(&virtualStress01);
	layer1.SetVirtualStress(&virtualStress01,&virtualStress12);
	layer2.SetVirtualStress(&virtualStress12);
	//layer3.SetVirtualStress(&virtualStress23);
	// 固定所有的边缘
	for (int i = 0; i < grid_size; i++)
	{
		layer0.GetMassRef(i).fixed = true;
		layer0.GetMassRef(i*grid_size).fixed = true;
		layer0.GetMassRef((i + 1)*grid_size - 1).fixed = true;
		layer0.GetMassRef(grid_size*grid_size - 1 - i).fixed = true;

		layer1.GetMassRef(i).fixed = true;
		layer1.GetMassRef(i*grid_size).fixed = true;
		layer1.GetMassRef((i + 1)*grid_size - 1).fixed = true;
		layer1.GetMassRef(grid_size*grid_size - 1 - i).fixed = true;

		layer2.GetMassRef(i).fixed = true;
		layer2.GetMassRef(i*grid_size).fixed = true;
		layer2.GetMassRef((i + 1)*grid_size - 1).fixed = true;
		layer2.GetMassRef(grid_size*grid_size - 1 - i).fixed = true;

		layer3.GetMassRef(i).fixed = true;
		layer3.GetMassRef(i*grid_size).fixed = true;
		layer3.GetMassRef((i + 1)*grid_size - 1).fixed = true;
		layer3.GetMassRef(grid_size*grid_size - 1 - i).fixed = true;
	}
	// 固定所有的点
	for (int i = 0; i < grid_size*grid_size; i++)
	{
		layer0.GetMassRef(i).fixed = true;
	}
	
	//layer.SetTexture("./ImageResource/1.jpg");
	//layer2.SetTexture("./ImageResource/3.jpg");
	
	renderer->AddActor(layer0.GetActor());
	renderer->AddActor(layer1.GetActor());
	renderer->AddActor(layer2.GetActor());
	renderer->AddActor(layer3.GetActor());

	// 在渲染窗口设置并显示坐标系
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

	// 计时器
	vtkSmartPointer<MyInteractorStyle> style = vtkSmartPointer<MyInteractorStyle>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	style->RegisterOnTimerFunc(UpdateFrame);

	style->SetUseTimers(30);
	style->UseTimersOn();

	while (1)
	{
		Sleep(50);
		UpdateFrame();
		renderWindow->Render();
	}


	//style->SetTimerDuration(30);
	//style->StartTimer();


	renderWindowInteractor->Start();

	system("pause");
	return 0;
}
void UpdateFrame()
{
	// 设置当前时间以及经过的时间
	static double lastTime = timer.GetTime();

	double currentTime = timer.GetTime();
	double timePassed = currentTime - lastTime;
	lastTime = currentTime;

	// 每隔10毫秒更新一次物理位置，以防止不同帧速率导致不同阻尼的问题。
	static double timeSinceLastUpdate = 0.0;
	timeSinceLastUpdate += timePassed;
	while (timeSinceLastUpdate > 10.0)
	{
		timeSinceLastUpdate -= 10.0;
		float timePassedInSeconds = 0.01f;
		layer0.UpdateFrame(timePassedInSeconds);
		layer1.UpdateFrame(timePassedInSeconds);
		layer2.UpdateFrame(timePassedInSeconds);
		layer3.UpdateFrame(timePassedInSeconds);
		layer2.PrintInfo();
	}
	
	//renderWindow->Render();
}




