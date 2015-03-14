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