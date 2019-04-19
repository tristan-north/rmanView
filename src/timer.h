/*
 * timer.h
 *
 *  Created on: May 21, 2016
 *      Author: ten
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>

class Timer {
public:
	Timer();
	float getElapsedMSec();
	void printElapsedMSec();

private:
	timespec startTs;
};

#endif /* TIMER_H_ */
