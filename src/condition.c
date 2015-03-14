#include "condition.h"

#include <sys/time.h>

int condition_init(struct condition* cond)
{
	if (pthread_mutex_init(&cond->mutex, NULL) < 0) {
		return -1;
	}

	if (pthread_cond_init(&cond->cond, NULL) < 0) {
		pthread_mutex_destroy(&cond->mutex);
		return -1;
	}

	return 0;
}

void condition_destroy(struct condition* cond)
{
	pthread_mutex_destroy(&cond->mutex);
	pthread_cond_destroy(&cond->cond);
}

void condition_wait(struct condition* cond)
{
	pthread_mutex_lock(&cond->mutex);
	pthread_cond_wait(&cond->cond, &cond->mutex);
	pthread_mutex_unlock(&cond->mutex);
}

void condition_wait_sec(struct condition* cond, int second)
{
	struct timeval now;
	struct timespec timeout;

	gettimeofday(&now, NULL);
	timeout.tv_sec = now.tv_sec + second;
	timeout.tv_nsec = 0;

	pthread_mutex_lock(&cond->mutex);
	pthread_cond_timedwait(&cond->cond, &cond->mutex, &timeout);
	pthread_mutex_unlock(&cond->mutex);
}

void condition_signal_one(struct condition* cond)
{
	pthread_cond_signal(&cond->cond);
}

void condition_signal_all(struct condition* cond)
{
	pthread_cond_broadcast(&cond->cond);
}

