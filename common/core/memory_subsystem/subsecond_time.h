#ifndef SUBSECOND_TIME_H__
#define SUBSECOND_TIME_H__
#include "fixed_types.h"
#include "lock.h"
#include <cassert>
#include <iostream>

class SubsecondTime
{
	public:
	   uint64_t m_time;
	   
	   static const SubsecondTime Zero() { return SubsecondTime(); }

	   // Public constructors
	   SubsecondTime()
	      : m_time(0)
	   {}
};

#endif /* SUBSECOND_TIME_H__ */
