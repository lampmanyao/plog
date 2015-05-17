#ifndef plog_h
#define plog_h

#include <stdio.h>

#define PLOG_LEVEL_ERROR   0
#define PLOG_LEVEL_WARNING 1
#define PLOG_LEVEL_INFO    2
#define PLOG_LEVEL_DEBUG   3

#define DEBUG(fmt, args ...) do { \
        fprintf(stderr, "[%s:%u:%s()] " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args); \
} while (0)

#define PLOG_DEBUG(args...) do { \
        plog_printf(3, __FILE__, __LINE__, args); \
} while (0)

#define PLOG_INFO(args...) do { \
        plog_printf(2, __FILE__, __LINE__, args); \
} while (0)

#define PLOG_WARNING(args...) do { \
        plog_printf(1, __FILE__, __LINE__, args); \
} while (0)

#define PLOG_ERROR(args...) do { \
        plog_printf(0, __FILE__, __LINE__, args); \
} while (0)

#define PLOG_FATAL(args...) do { \
        plog_fatal(__FILE__, __LINE__, args); \
} while (0)

int plog_open(const char* logname, int level, long rotate_size);
void plog_printf(int level, const char* filename, int lineno, const char* fmt, ...);
void plog_fatal(const char* filename, int lineno, const char* fmt, ...);
void plog_close();

#endif /* plog_h */

