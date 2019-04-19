/*
 * timer.cpp
 *
 *  Created on: May 21, 2016
 *      Author: ten
 */

#include <stdio.h>
#include "timer.h"

Timer::Timer() {
	clock_gettime(CLOCK_MONOTONIC, &startTs);
}

float Timer::getElapsedMSec() {
	timespec endTs;
	clock_gettime(CLOCK_MONOTONIC, &endTs);

	unsigned long startNanoSec = startTs.tv_sec*1000000000 + startTs.tv_nsec;
	unsigned long endNanoSec = endTs.tv_sec*1000000000 + endTs.tv_nsec;

	return (endNanoSec - startNanoSec) / 1000000.0;
}


void Timer::printElapsedMSec() {
	printf("%.2f ms\n", getElapsedMSec());
}
