/*
* Copyright 2008 Free Software Foundation, Inc.
*
* This software is distributed under the terms of the GNU Affero Public License.
* See the COPYING file in the main directory for details.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef TIMEVAL_H
#define TIMEVAL_H

#include <stdint.h>
#include "sys/time.h"
#include <iostream>
#include <unistd.h>

inline void msleep(long v) { usleep((v+500)/1000); }


class SysTime{

	private:

		time_t mNowTime;
		struct timeval mTimeval;
		struct tm fields;
		char mMessage[32];
		unsigned mCount;


	public:

		SysTime()
			:mNowTime(time(NULL))
		{ now();}

		SysTime(unsigned wCount)
			:mNowTime(time(NULL)), mCount(wCount)
		{now();}

		SysTime(char *wMessage)
			{
			strptime(wMessage, "%Y-%m-%d %H:%M:%S", &fields);
			mNowTime = mktime(&fields);
			strftime(mMessage, 40, "%Y-%m-%d %H:%M:%S",&fields);
			}

		void now ()
			{
			gettimeofday(&mTimeval,NULL);
			localtime_r(&mNowTime,&fields);
			strftime(mMessage, 40, "%Y-%m-%d %H:%M:%S",&fields);
			}

		void future(unsigned offset)
		{
			now();
			unsigned sec = offset/1000;
			unsigned msec = offset%1000;
			mTimeval.tv_usec += msec*1000;
			mTimeval.tv_sec += sec;
			if (mTimeval.tv_usec>1000000) {
				mTimeval.tv_usec -= 1000000;
				mTimeval.tv_sec += 1;
			}
		}

		double seconds() const
		{
			return ((double)mTimeval.tv_sec) + 1e-6*((double)mTimeval.tv_usec);
		}

		uint32_t sec() const { return mTimeval.tv_sec; }
		uint32_t usec() const { return mTimeval.tv_usec; }


		long delta(const SysTime& other) const
		{
			int32_t deltaS = other.sec() - sec();
       		int32_t deltaUs = other.usec() - usec();
			return 1000*deltaS + deltaUs/1000;
		}

		/** Elapsed time in ms. */
		long elapsed() const { return delta(SysTime()); }

		/** Remaining time in ms. */
		long remaining() const { return -elapsed(); }

		/** Return true if the time has passed, as per gettimeofday. */
		bool passed() const
		{
			SysTime nowTime;
			if (nowTime.mTimeval.tv_sec < mTimeval.tv_sec) return false;
			if (nowTime.mTimeval.tv_sec > mTimeval.tv_sec) return true;
			if (nowTime.mTimeval.tv_usec > mTimeval.tv_usec) return true;
			return false;
		}


		const unsigned GetCount() const {return mCount;}
		void SetCount(unsigned wCount) {mCount=wCount;}
		/* 将字符串转换成time_t：

		char time_str[128] = "2005-10-18 9:37:45";
		struct tm tmp;
		strptime(time_str, "%Y-%m-%d %H:%M:%S", &tmp);
		time_t tm = mktime(&tmp);


		将time_t转换成字符串：

		time_t tm = time(NULL);
		struct tm tmp;
		localtime_r(&tm, &tmp);
		strftime(buffer, bufferlen, "%Y-%m-%d %H:%M:%S", &tmp);
		*/
		const char * message() const {return mMessage;}
		time_t getTime() {return mNowTime;};

};



/** A C++ wrapper for struct timeval. */
class Timeval {

	private:

	struct timeval mTimeval;

	public:

	/** Set the value to gettimeofday. */
	void now() { gettimeofday(&mTimeval,NULL); }

	/** Set the value to gettimeofday plus an offset. */
	void future(unsigned ms);

	//@{
	Timeval(unsigned sec, unsigned usec)
	{
		mTimeval.tv_sec = sec;
		mTimeval.tv_usec = usec;
	}

	Timeval(const struct timeval& wTimeval)
		:mTimeval(wTimeval)
	{}

	/**
		Create a Timeval offset into the future.
		@param offset milliseconds
	*/
	Timeval(unsigned offset=0) { future(offset); }
	//@}

	/** Convert to a struct timespec. */
	struct timespec timespec() const;

	/** Return total seconds. */
	double seconds() const;

	uint32_t sec() const { return mTimeval.tv_sec; }
	uint32_t usec() const { return mTimeval.tv_usec; }

	/** Return differnce from other (other-self), in ms. */
	long delta(const Timeval& other) const;

	/** Elapsed time in ms. */
	long elapsed() const { return delta(Timeval()); }

	/** Remaining time in ms. */
	long remaining() const { return -elapsed(); }

	/** Return true if the time has passed, as per gettimeofday. */
	bool passed() const;

	/** Add a given number of minutes to the time. */
	void addMinutes(unsigned minutes) { mTimeval.tv_sec += minutes*60; }

};

std::ostream& operator<<(std::ostream& os, const Timeval&);

std::ostream& operator<<(std::ostream& os, const SysTime&);

std::ostream& operator<<(std::ostream& os, const struct timespec&);


#endif
// vim: ts=4 sw=4
