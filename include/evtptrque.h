#ifndef _EVTPTRQUE_H_
#define _EVTPTRQUE_H_


class Event;

class EventPtrQueue
{
public:
	virtual void post(Event* evt) = 0;
	virtual bool isEmpty(void) = 0;
	virtual Event* peek(void) = 0;
	virtual Event* get(void) = 0;
};

EventPtrQueue* createEventPtrQueue(void);
void destroyEventPtrQueue(EventPtrQueue* queue);

#endif /* _EVTPTRQUE_H_ */

