#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef ENABLE_DEBUG_PRINT

#define ASSERT(x)			assert(x)

#define LOG_DEBUG(x)		(std::cout << "DEBUG:" << x << std::endl)
#define LOG_ERROR(x)		(std::cout << "ERROR:" << x << std::endl)

#else

#define ASSERT(x)

#define LOG_DEBUG(x)
#define LOG_ERROR(x)

#endif

#endif /* _DEBUG_H_ */
