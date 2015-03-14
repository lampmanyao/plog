#ifndef condition_h
#define condition_h

#include <pthread.h>

struct condition {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

int condition_init(struct condition* cond);
void condition_destroy(struct condition* cond);

void condition_wait(struct condition* cond);
void condition_wait_sec(struct condition* cond, int second);
void condition_signal_one(struct condition* cond);
void condition_signal_all(struct condition* cond);

#endif  /* condition_h */

