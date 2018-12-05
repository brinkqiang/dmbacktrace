
#ifndef WIN32

#include <stacktrace.h>
#include "dmformat.h"

extern "C" void _stacktrace_set_exc();
extern "C" struct stacktrace *_stacktrace_get_exc();

extern "C" struct stacktrace *stacktrace_get_exc() {
    return _stacktrace_get_exc();
}

extern "C" void __cxa_throw(void *thrown_exception, std::type_info *tinfo, void (*dest)(void *))
        __attribute__(( noreturn ));

extern "C" void __wrap___cxa_throw(void *thrown_exception, std::type_info *tinfo, void (*dest)(void *)) {
    _stacktrace_set_exc();
    __cxa_throw(thrown_exception, tinfo, dest);
}

void stacktrace_string(struct stacktrace *trace, std::string* str)
{
    int i;

    stacktrace_resolve(trace);

    for (i = 0; i < trace->frames_len; i++) {
        struct stacktrace_frame *frame = &trace->frames[i];
        std::string strLine;
        fmt::format(strLine, "#%d %p - %s in %s:%d\n", i, frame->addr,
            frame->func ? frame->func : "??", frame->file ? frame->file : "??", frame->line);
        str->append(strLine);
    }
}
#endif
