#ifndef __STACKTRACE_H_INCLUDE__
#define __STACKTRACE_H_INCLUDE__

#ifndef WIN32

#include <stdio.h>
#include <string>
struct stacktrace;

struct stacktrace *stacktrace_get(unsigned skip);
void stacktrace_resolve(struct stacktrace *st);
void stacktrace_free(struct stacktrace *st);

void stacktrace_print(struct stacktrace *st);
void stacktrace_fprint(struct stacktrace *st, FILE *);
void stacktrace_string(struct stacktrace *st, std::string* str);
struct stacktrace *stacktrace_get_exc();

void _stacktrace_set_exc();
struct stacktrace *_stacktrace_get_exc();

struct stacktrace *stacktrace_get_exc();

extern "C" void __cxa_throw(void *thrown_exception, std::type_info *tinfo, void(*dest)(void *))
__attribute__((noreturn));

extern "C" void __wrap___cxa_throw(void *thrown_exception, std::type_info *tinfo, void(*dest)(void *)) {
    _stacktrace_set_exc();
    __cxa_throw(thrown_exception, tinfo, dest);
}

void stacktrace_string(struct stacktrace *trace, std::string* str);

#endif
#endif // __STACKTRACE_H_INCLUDE__
