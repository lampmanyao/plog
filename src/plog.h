#ifndef plog_h
#define plog_h

#include <stdio.h>

#define PLOG_LEVEL_ERROR   0
#define PLOG_LEVEL_WARNING 1
#define PLOG_LEVEL_INFO    2
#define PLOG_LEVEL_DEBUG   3

void _format_time(char* time_buff);

#define DEBUG(fmt, args ...) do { \
        fprintf(stderr, "[%s:%u:%s()] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args); \
} while (0)

#define PLOG_DEBUG(fmt, args ...) do { \
	char time_fmt[32]; \
	_format_time(time_fmt); \
	plog_printf(3, "%s %s:%d DEBUG " fmt "\n", time_fmt, __FILE__, __LINE__, ##args); \
} while (0)

#define PLOG_INFO(fmt, args...) do { \
	char time_fmt[32]; \
	_format_time(time_fmt); \
	plog_printf(2, "%s %s:%d INFO " fmt "\n", time_fmt, __FILE__, __LINE__, ##args); \
} while (0)

#define PLOG_WARNING(fmt, args...) do { \
	char time_fmt[32]; \
	_format_time(time_fmt); \
	plog_printf(1, "%s %s:%d WARNING " fmt "\n", time_fmt, __FILE__, __LINE__, ##args); \
} while (0)

#define PLOG_ERROR(fmt, args...) do { \
	char time_fmt[32]; \
	_format_time(time_fmt); \
	plog_printf(0, "%s %s:%d ERROR " fmt "\n", time_fmt, __FILE__, __LINE__, ##args); \
} while (0)

#define PLOG_FATAL(fmt, args...) do { \
	char time_fmt[32]; \
	_format_time(time_fmt); \
        plog_fatal("%s %s:%d FATAL " fmt "\n", time_fmt, __FILE__, __LINE__, ##args); \
} while (0)

int plog_open(const char* logname, int level, long rotate_size);
void plog_printf(int level, const char *fmt, ...);
void plog_fatal(const char* fmt, ...);
void plog_close();

#endif /* plog_h */

