#pragma comment( lib,"winmm.lib" )
#ifndef TIMER_H
#define TIMER_H

class TIMER
{
public:
	TIMER()	:	isPaused(false)
	{	Reset();	}
	~TIMER()	{}

	void Reset();
	double GetTime();

	void Pause();
	void Unpause();

protected:
	double startTime;

	bool isPaused;
	double pauseTime;
};

#endif	//TIMER_H