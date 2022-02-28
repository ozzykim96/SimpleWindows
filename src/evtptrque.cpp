#include "windows.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <queue>

// reference http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

class ThreadEventPtrQueue : public EventPtrQueue
{
public:
	virtual void post(Event* evt);
	virtual bool isEmpty(void);
	virtual Event* peek(void);
	virtual Event* get(void);

private:
	std::queue<Event*> the_queue;
	mutable boost::mutex the_mutex;
	boost::condition_variable the_condition_variable;
};

void ThreadEventPtrQueue::post(Event* evt)
{
	boost::mutex::scoped_lock lock(the_mutex);
	the_queue.push(evt);
	lock.unlock();
	the_condition_variable.notify_one();
}

bool ThreadEventPtrQueue::isEmpty(void)
{
	boost::mutex::scoped_lock lock(the_mutex);
	return the_queue.empty();
}

Event* ThreadEventPtrQueue::peek(void)
{
	boost::mutex::scoped_lock lock(the_mutex);
	if(the_queue.empty())
	{
		return NULL;
	}

	Event* evt = the_queue.front();
	the_queue.pop();
	return evt;
}

Event* ThreadEventPtrQueue::get(void)
{
	boost::mutex::scoped_lock lock(the_mutex);
	while(the_queue.empty())
	{
		the_condition_variable.wait(lock);
	}

	Event* evt = the_queue.front();
	the_queue.pop();
	return evt;
}

EventPtrQueue* createEventPtrQueue(void)
{
	return new ThreadEventPtrQueue;
}

void destroyEventPtrQueue(EventPtrQueue* queue)
{
	ASSERT(queue);
	delete queue;
}
