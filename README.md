# plog
A high performance, thread safe, pure c logging library.

## How to use
Just copy the files to your build tree and use a C compiler

## APIs
### functions
plog_open() and plog_close() *MUST BE* called at main thread.

### Macros
PLOG_XXXX() could be called *EVERYWHERE* at your code.

See test/ for more details

## Benchmarks
threads | lines per thread | total lines | seconds |   
--------+------------------+-------------+----------   
1       | 10,000,000       | 10,000,000  | 10      |   
2       | 10,000,000       | 20,000,000  | 15      |   
3       | 10,000,000       | 30,000,000  | 18      |   
10      | 10,000,000       | 100,000,000 | 70      |   
