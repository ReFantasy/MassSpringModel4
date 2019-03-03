#ifndef __MYINTERACTORSTYLE_H__
#define __MYINTERACTORSTYLE_H__
#include "vtkheader.h"
#include <cmath>
typedef void(*OnTimerFunc)();

class MyInteractorStyle :public vtkInteractorStyle
{
public:
	MyInteractorStyle()
	{
		this->MotionFactor = 10.0;
		UpdateFrame = nullptr;
	}
	
	static MyInteractorStyle*New()
	{
		return new MyInteractorStyle;
	}
	
	void OnTimer()override
	{
		if(UpdateFrame)
			UpdateFrame();
	}
	void RegisterOnTimerFunc(OnTimerFunc func) { UpdateFrame = func; }

	//////////////////////////////////////////////////////////////////////////
	void OnMouseMove() override;
	void OnLeftButtonDown() override;
	void OnLeftButtonUp() override;
	void OnMiddleButtonDown() override;
	void OnMiddleButtonUp() override;
	void OnRightButtonDown() override;
	void OnRightButtonUp() override;
	void OnMouseWheelForward() override;
	void OnMouseWheelBackward() override;

	void Rotate() override;
	void Spin() override;
	void Pan() override;
	void Dolly() override;

	double MotionFactor;

	virtual void Dolly(double factor);
private:
	OnTimerFunc UpdateFrame;
};
#endif //__MYINTERACTORSTYLE_H__
