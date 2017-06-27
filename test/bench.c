#include "plog.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int nloop = 0;

void* log_thread(void* arg)
{
	pthread_t tid = pthread_self();
	int i = 0;
	for (; i < nloop; i++) {
		PLOG_DEBUG("this is thread[%ld]: %d", tid, i);
	}

	printf("thread[%ld] exit\n", (long)tid);
	return NULL;
}

int main(int argc, char** argv)
{
	if (argc != 4) {
		printf("usage:\n");
		printf("\t%s logname nthreads nlogperthread\n", argv[0]);
		return -1;
	}

	if (plog_open(argv[1], PLOG_LEVEL_DEBUG, 1024 * 1024 * 5) < 0)
		return -1;

	int n = atoi(argv[2]);
	nloop = atoi(argv[3]);
	pthread_t* tids = calloc(n, sizeof(pthread_t));

	int i;
	for (i = 0; i < n; i++) {
		pthread_create(&tids[i], NULL, log_thread, NULL);
	}

	for (i = 0; i < n; i++) {
		pthread_join(tids[i], NULL);
	}
	plog_close();

	return 0;
}

