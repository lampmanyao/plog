#include "plog.h"
#include "condition.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdarg.h>
#include <pthread.h>

#define BUF_SIZE (1024 * 1024)  /* 1M easy for rotate */
#define BUF_LEN  1024
#define N_ELEMENTS 100

#define LOCK() while (__sync_lock_test_and_set(&__plog.lock, 1))
#define UNLOCK() __sync_lock_release(&__plog.lock)

struct plog_buffer {
	int seek;
	int remain;
	char data[BUF_SIZE];
	struct plog_buffer* next;
};

struct plog {
	int level;
	int lock;
	int rotate_size;
	int fseek;
	int fd;
	time_t last_sec;
	char last_timestamp[18];
	char logname[BUF_LEN];
	const char* info[4];
	struct condition condition;
	struct plog_buffer* head;
	struct plog_buffer* tail;
	struct plog_buffer** buffers;
};

struct plog __plog = {
	.level = -1,
};

static struct plog_buffer* _malloc_plog_buffer()
{
	struct plog_buffer* b = NULL;
	b = malloc(sizeof(struct plog_buffer));
	b->seek = 0;
	b->remain = BUF_SIZE;
	b->next = NULL;
	return b;
}

static void _append(struct plog_buffer* plog_buffer, const char* msg, int _len)
{
	memcpy(plog_buffer->data + plog_buffer->seek, msg, _len);
	plog_buffer->seek += _len;
	plog_buffer->remain -= _len;
}

static struct plog_buffer** _create_plog_buffers()
{
	int i;
	struct plog_buffer** arr = calloc(N_ELEMENTS, sizeof(struct plog_buffer_list*));
	for (i = 0; i < N_ELEMENTS; i++) {
		struct plog_buffer* b = _malloc_plog_buffer();
		arr[i] = b;
	}
	arr[N_ELEMENTS - 1]->next = arr[0];
	for (i = 0; i < N_ELEMENTS - 1; i++) {
		arr[i]->next = arr[i + 1];
	}

	return arr;
}

static void _destroy_plog_buffers(struct plog_buffer** _arr)
{
	int i;
	for (i = 0; i < N_ELEMENTS; i++) {
		free(_arr[i]);
	}

	free(_arr);
}

static void _create_new_file()
{
	close(__plog.fd);

	char path[2048];
	struct timeval tv;
	struct tm tm;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);

	snprintf(path, 2048, "%s-%04d%02d%02d-%02d-%02d-%02d-%06d.log",
		__plog.logname,
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
		tm.tm_min, tm.tm_sec, (int)tv.tv_usec);

	__plog.fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0644);

	if (__plog.fd == -1) {
		perror("open");
		return;
	}

	__plog.fseek = 0;
}

static void _format_time(char* time_buff)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	if (tv.tv_sec != __plog.last_sec) {
		struct tm tm;
		localtime_r(&tv.tv_sec, &tm);
		snprintf(__plog.last_timestamp, 18, "%04d%02d%02d %02d:%02d:%02d",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
		__plog.last_sec = tv.tv_sec;
	}

	sprintf(time_buff, "%s.%06d", __plog.last_timestamp, (int)tv.tv_usec);
}

void* thread_flush(void* arg)
{
	while (1) {
		if (__plog.head == __plog.tail) {
			condition_wait_sec(&__plog.condition, 1);
		}

		LOCK();
		__plog.fseek += write(__plog.fd, __plog.head->data, __plog.head->seek);
		if (__plog.fseek >= __plog.rotate_size) {
			_create_new_file();
		}
		__plog.head->seek = 0;
		__plog.head->remain = BUF_SIZE;
		__plog.head = __plog.head->next;
		UNLOCK();

	}

	return NULL;
}

int plog_open(const char* logname, int level, long rotate_size)
{
	__plog.lock = 0;
	__plog.level = level;
	__plog.rotate_size = rotate_size;
	__plog.fseek = 0;

	strncpy(__plog.logname, logname, BUF_LEN);

	char path[2048] = {0};
	struct timeval tv;
	struct tm tm;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);

	snprintf(path, 2048, "%s-%04d%02d%02d-%02d-%02d-%02d-%06d.log",
		__plog.logname,
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec, (int)tv.tv_usec);

	__plog.fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0644);

	if (__plog.fd == -1) {
		perror("open");
		return -1;
	}

	__plog.last_sec = tv.tv_sec;
	snprintf(__plog.last_timestamp, 18, "%04d%02d%02d %02d:%02d:%02d",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);


	__plog.info[0] = " ERROR ";
	__plog.info[1] = " WARNING ";
	__plog.info[2] = " INFO ";
	__plog.info[3] = " DEBUG ";

	condition_init(&__plog.condition);

	__plog.buffers = _create_plog_buffers();
	__plog.head = __plog.buffers[0];
	__plog.tail = __plog.buffers[0];

	pthread_t tid;
	if (pthread_create(&tid, NULL, thread_flush, NULL) < 0) {
		perror("pthread_create");
		close(__plog.fd);
		_destroy_plog_buffers(__plog.buffers);
		return -1;
	}

	return 0;
}

void plog_printf(int level, const char* filename, int lineno, const char* fmt, ...)
{
	if (level > __plog.level)
		return;

	char buf[BUF_LEN * 2] = {0};
	char user_msg[BUF_LEN] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(user_msg, BUF_LEN, fmt, ap);
	va_end(ap);

	char time_fmt[32];
	_format_time(time_fmt);

	int len = snprintf(buf, BUF_LEN * 2, "%s %s:%d%s: %s\n", time_fmt,
			filename, lineno, __plog.info[level], user_msg);

	LOCK();
	if (__plog.tail->remain > len) {
		_append(__plog.tail, buf, len);
	} else {
		__plog.tail = __plog.tail->next;
		condition_signal_one(&__plog.condition);
		_append(__plog.tail, buf, len);
	}
	UNLOCK();
}

void plog_fatal(const char* filename, int lineno, const char* fmt, ...)
{
	char buf[BUF_LEN * 2] = {0};
	char user_msg[BUF_LEN] = {0};
	va_list ap; 

	va_start(ap, fmt);
	vsnprintf(user_msg, BUF_LEN, fmt, ap);
	va_end(ap);

	char time_fmt[32];
	_format_time(time_fmt);

	int len = snprintf(buf, BUF_LEN * 2, "%s %s:%d FATAL : %s\n", time_fmt,
			filename, lineno, user_msg);

	LOCK();
	__plog.fseek = write(__plog.fd, buf, len);
	UNLOCK();
}

void plog_close()
{
	close(__plog.fd);
	condition_destroy(&__plog.condition);
	_destroy_plog_buffers(__plog.buffers);
}
