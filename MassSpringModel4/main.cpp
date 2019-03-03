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
int grid_size = 13;
Layer layer(grid_size);
Layer layer2(grid_size, 1);
VirtualStress virtualStress(&layer, &layer2);


int main()
{
	layer.SetVirtualStress(&virtualStress);
	layer2.SetVirtualStress(&virtualStress);

	// 固定所有的边缘
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
	renderer->AddActor(layer.GetActor());
	renderer->AddActor(layer2.GetActor());

	renderWindow->AddRenderer(renderer);

	renderWindowInteractor->SetRenderWindow(renderWindow);

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
		layer.UpdateFrame(timePassedInSeconds);
		layer2.UpdateFrame(timePassedInSeconds);
	}
	
	//renderWindow->Render();
}




