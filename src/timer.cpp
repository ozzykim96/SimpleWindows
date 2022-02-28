#include "windows.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time.hpp>

enum
{
	TIMER_ONCE,
	TIMER_REPEAT
};

class TimerItem
{
public:
	TimerItem(TimerId id, int type, unsigned int millisec, boost::asio::deadline_timer* timer) 
		: m_id(id), m_type(type), m_ms(millisec), m_timer(timer) {}
	~TimerItem(void)
	{
		delete m_timer;
	}
	
	TimerId getId(void) { return m_id; }
	int getType(void) { return m_type; }
	unsigned int getMillisec(void) { return m_ms; }
	boost::asio::deadline_timer* getTimer(void) { return m_timer; }
private:
	TimerId m_id;
	int m_type;
	unsigned int m_ms;
	boost::asio::deadline_timer* m_timer;
};

class BoostTimer : public Timer
{
public:
	BoostTimer(void);
	~BoostTimer(void);

	virtual bool start(void);
	virtual void stop(void);

	virtual TimerId setOnce(unsigned int millisec);
	virtual TimerId setRepeat(unsigned int millisec);
	virtual void remove(TimerId id);
	
	void timerCallback(TimerId id);
private:
	TimerItem* _createItem(int type, unsigned int millisec);
	TimerItem* _getItem(unsigned int id);
	TimerId _getNextId(void);
	
	static unsigned int m_timerIdCnt;
	std::vector<TimerItem*> m_timerList;
	
	boost::asio::io_service m_io;
	boost::asio::io_service::work m_work;
	
	boost::mutex m_mutex;
};

unsigned int BoostTimer::m_timerIdCnt = 0;

BoostTimer::BoostTimer(void) : m_io(), m_work(m_io)
{
}

BoostTimer::~BoostTimer(void)
{
	for (std::vector<TimerItem*>::iterator it = m_timerList.begin(); it != m_timerList.end(); ++it)
	{
		delete (*it);
	}	
	
	m_io.stop();
}

class AsioThread
{
public:
	AsioThread(boost::asio::io_service* io) : m_io(io) {}
	void operator()()
	{
		LOG_DEBUG("[timer] thread started.");
		
		m_io->run();
		
		LOG_DEBUG("[timer] thread ended.");
	}
	
	boost::asio::io_service* m_io;
};

bool BoostTimer::start(void)
{
	// create io thread
	AsioThread ioThrd(&m_io);
	boost::thread thrd(ioThrd);
	
	return true;
}

void BoostTimer::stop(void)
{
	// destroy thread

}

TimerId BoostTimer::setOnce(unsigned int millisec)
{
	boost::mutex::scoped_lock lock(m_mutex);

	// create timer item
	TimerItem* item = _createItem(TIMER_ONCE, millisec);
	return item->getId();
}

TimerId BoostTimer::setRepeat(unsigned int millisec)
{
	boost::mutex::scoped_lock lock(m_mutex);

	// create timer item
	TimerItem* item = _createItem(TIMER_REPEAT, millisec);
	return item->getId();
}

void BoostTimer::remove(unsigned int id)
{
	boost::mutex::scoped_lock lock(m_mutex);
	
	for (unsigned int i = 0; i < m_timerList.size(); i++)
	{
		if (m_timerList[i]->getId() == id)
		{
			delete m_timerList[i];
			m_timerList.erase(m_timerList.begin() + i);
			
			LOG_DEBUG("[timer:" << id << "] removed.");
			break;
		}
	}
}

void BoostTimer::timerCallback(TimerId id)
{
	boost::mutex::scoped_lock lock(m_mutex);

	TimerItem* item = _getItem(id);
	if (!item)
	{
		// no item
		return;
	}
	
	LOG_DEBUG("[timer:" << item->getId() << "," << item->getMillisec() << "] fired.");
	
	// generate event
	getTimerEventListener()->onTimerEvent(item->getId());
	
	// if repeat timer, start again.
	if (item->getType() == TIMER_REPEAT)
	{
		LOG_DEBUG("[timer:" << item->getId() << "] rescheduled.");

		boost::asio::deadline_timer* timer = item->getTimer();
		
		timer->expires_at(timer->expires_at() + boost::posix_time::milliseconds(item->getMillisec()));
		timer->async_wait(boost::bind(&BoostTimer::timerCallback, this, item->getId()));
	}
}

TimerItem* BoostTimer::_createItem(int type, unsigned int millisec)
{
	TimerId id = _getNextId();

	LOG_DEBUG("[timer:" << id << "] added.");
	
	boost::asio::deadline_timer* timer = new boost::asio::deadline_timer(m_io, boost::posix_time::milliseconds(millisec));
	
	TimerItem* item = new TimerItem(id, type, millisec, timer);
	m_timerList.push_back(item);
		
	// start timer
	timer->async_wait(boost::bind(&BoostTimer::timerCallback, this, item->getId()));

	return item;
}

TimerId BoostTimer::_getNextId(void)
{
	// TODO : create unique id
	m_timerIdCnt++;
	if (m_timerIdCnt == 0)
		m_timerIdCnt = 1;
	
	return (TimerId)m_timerIdCnt;
}

TimerItem* BoostTimer::_getItem(TimerId id)
{
	for (std::vector<TimerItem*>::iterator it = m_timerList.begin(); it != m_timerList.end(); ++it)
	{
		if ((*it)->getId() == id)
			return (*it);
	}
	return NULL;
}

Timer* createTimer(void)
{
	return new BoostTimer;
}

void destroyTimer(Timer* timer)
{
	ASSERT(timer);
	delete timer;
}

