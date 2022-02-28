#ifndef _TIMER_H_
#define _TIMER_H_

typedef unsigned int TimerId;

class TimerEventListener
{
public:
	virtual void onTimerEvent(TimerId id) = 0;
};

class Timer
{
public:
	virtual bool start(void) = 0;
	virtual void stop(void) = 0;

	virtual TimerId setOnce(unsigned int millisec) = 0;
	virtual TimerId setRepeat(unsigned int millisec) = 0;
	virtual void remove(TimerId timerId) = 0;
	
	void setTimerEventListener(TimerEventListener* evtListener) { m_evtListener = evtListener; }
	TimerEventListener* getTimerEventListener(void) { return m_evtListener; }
private:
	TimerEventListener* m_evtListener;
};

// TODO: need factory class
Timer* createTimer(void);
void destroyTimer(Timer* timer);

#endif /* _TIMER_H_ */
