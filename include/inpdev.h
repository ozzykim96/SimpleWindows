#ifndef _INPDEV_H_
#define _INPDEV_H_

#include "keycode.h"

enum InputKeyEvent
{
	KEY_PRESSED,
	KEY_RELEASE
};

class InputEventListener
{
public:
	virtual void onKeyEvent(InputKeyEvent event, KeyCode code) = 0;
};

class InputDevice
{
public:
	virtual bool init(void) = 0;
	virtual void shutdown(void) = 0;

	virtual bool listen(void) = 0;
	virtual bool stop(void) = 0;	
	
	void setInputEventListener(InputEventListener* evtListener) { m_evtListener = evtListener; }
	InputEventListener* getInputEventListener(void) { return m_evtListener; }	
private:
	InputEventListener* m_evtListener;
};

// TODO: need factory class
InputDevice* createInputDevice(void);
void destroyInputDevice(InputDevice* device);

#endif /* _INPDEV_H_ */

