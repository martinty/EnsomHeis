#include "timer.h"
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

static struct timeval startTime;

void t_timer_start(void){
	gettimeofday(&startTime, NULL);
}

int t_check_if_timer_is_finished(double sek){
	static struct timeval endTime;
	gettimeofday(&endTime, NULL);
	double secs = (endTime.tv_sec - startTime.tv_sec) * 1000
	+ (endTime.tv_usec - startTime.tv_usec) / 1000;
	if (secs/1000 == sek){
		return 1;
	}
	return 0;
}

